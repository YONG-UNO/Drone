//
// Created by DingYong on 25-8-8.
//


#include "pid.h"

#include <stddef.h>

#include "limit.h"

void pidInit(pid_t *pid, float const kp, float const ki, float const kd,
             float const max_out, float const max_output_i) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_output   = max_out;
    pid->max_output_i = max_output_i;

    // 初始化内部状态
    pid->target   = 0;
    pid->feedback = 0;
    pid->output   = 0;

    pid->p_term = 0;
    pid->i_term = 0;
    pid->d_term = 0;

    pid->error[0] = pid->error[1] = pid->error[2] = 0;
}

void pidReset(pid_t *pid) {
    if (pid == NULL) return;   // 空指针保护,避免崩溃

    // 重置PID输出及分项
    pid->output  = 0.0f;
    pid->p_term  = 0.0f;
    pid->i_term  = 0.0f;       // 关键: 积分项必须清零, 否则会残留输出
    pid->d_term  = 0.0f;

    // 重置误差历史 (避免微分项残留)
    pid->error[0] = 0.0f;
    pid->error[1] = 0.0f;
    pid->error[2] = 0.0f;
}

/**
 * @brief 重置串级PID（角度环+速度环）的所有状态，确保输出完全清零
 * @param angle_pid 角度环PID结构体指针
 * @param speed_pid 速度环PID结构体指针
 * @use   pidCasecadeRset(&angle_pid, &speed_pid);
 */
void pidCascadeReset(pid_t *angle_pid, pid_t *speed_pid) {
    // 分别重置两个环(内部已经包含空指针保护)
    pidReset(angle_pid);
    pidReset(speed_pid);
}

/**
 * @brief pid角度环
 * @todo 添加增量式
 * @fixme 添加空指针检测
 * @param pid
 * @param target
 * @param feedback
 * @return
 */
float pidAngle(pid_t *pid, float const target, float const feedback) {
    // 更新error
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    pid->target   = target;
    pid->feedback = feedback;


    // 过零保护:先修正feedback,再计算error[0],逻辑顺序
    pidOverZero_8192(&pid->target, &pid->feedback);

    pid->error[0] = pid->target - pid->feedback;


    pid->p_term =  pid->kp * pid->error[0];                    // 比例项
    pid->i_term += pid->ki * pid->error[0];                    // 积分项
    pid->d_term =  pid->kd * (pid->error[0] - pid->error[1]);  // 微分项

    // max_output_i
    pid->i_term = setOutLimit(pid->i_term, pid->max_output_i, -pid->max_output_i);

    // output
    pid->output = pid->p_term + pid->i_term + pid->d_term;

    // max_output
    pid->output = setOutLimit(pid->output, pid->max_output, -pid->max_output);

    return pid->output;
}

float pidSpeed(pid_t *pid, const float target, const float feedback) {
    // 更新error
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = target - feedback;

    pid->target   = target;
    pid->feedback = feedback;

    pid->p_term =  pid->kp * pid->error[0];                    // 比例项
    pid->i_term += pid->ki * pid->error[0];                    // 积分项
    pid->d_term =  pid->kd * (pid->error[0] - pid->error[1]);  // 微分项

    // max_output_i
    pid->i_term = setOutLimit(pid->i_term, pid->max_output_i, -pid->max_output_i);

    // output
    pid->output = pid->p_term + pid->i_term + pid->d_term;

    // max_output
    pid->output = setOutLimit(pid->output, pid->max_output, -pid->max_output);

    return pid->output;
}

/**
 * @brief 串级pid控制 (角度环 -> 角速度环 -> 电流环) (位置环 -> 速度环 -> 电流环)
 *
 *         *背景介绍:当我想出以角度环为内环妄想提升电机角度响应之后
 *         *角度的微分是速度,速度的微分是加速度(也就是力矩或者电流)
 *         *也就是说如果三个环的话,从外到内一定是角度,速度,电流 ---- 鸭哥原话,熟读并背诵
 *         *
 *         *pid 输入输出关系不是简单的微分关系,但可以从这个角度理解;
 *         *且理论上似乎pid控制器可以单极稳定控制二阶系统      ---- 队长原话,熟读并背诵
 *         *
 *         *自控学习路径: 高数 线代 复变函数与积分变换
 *         *            控制之美 杭电自控教材 东大自控教材
 *         *
 *
 *
 * @brief  串级PID计算封装（外环：角度环，内环：速度环）
 * @details 封装了「角度PID计算目标速度 → 速度PID计算目标力矩/电流」的完整流程，
 *          简化上层调用，返回最终可直接用于驱动电机的输出值（力矩/电流）
 * @param  angle_pid      角度PID（外环）结构体指针
 * @param  speed_pid      速度PID（内环）结构体指针
 * @param  angle_target   目标角度
 * @param  angle_feedback 实际角度反馈（来自电机编码器，单位：rad）
 * @param  speed_feedback 实际速度反馈（来自电机编码器，单位：rad/s）
 * @retval float          最终输出值
 */
float pidCascade(pid_t *angle_pid, pid_t *speed_pid,
                 const float angle_target, float const angle_feedback,
                 float const speed_feedback) {
    // pidAngle 计算得到 pidSpeed 的target
    float speed_target = pidAngle(angle_pid, angle_target, angle_feedback);

    // pidAngle 计算得到最后电流值(扭矩)
    float current = pidSpeed(speed_pid, speed_target, speed_feedback);

    return current;
}

/**
 * @brief 过零保护(8192版本)
 *        角度Pid时，在更新 target和 feedback之后紧接着调用, 处理完再进行PID计算
 * @link  https://www.codeleading.com/article/15905617492/\endlink
 * @param target
 * @param feedback
 */
void pidOverZero_8192(const float *target, float *feedback) {
    if      (*target - *feedback > 4096)    *feedback += 8192;   // 4096:半圈机械角度
    else if (*target - *feedback < -4096)   *feedback -= 8192;
    else    {/* *feedback = *feedback */}
}