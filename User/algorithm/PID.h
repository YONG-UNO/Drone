//
// Created by DingYong on 25-8-8.
//

#ifndef PID_H
#define PID_H

typedef struct {
    // 控制参数
    float kp;                // 比例系数
    float ki;                // 积分系数
    float kd;                // 微分系数

    // 输出限制
    float max_output;        // 最大输出限制
    float max_output_i;      // 积分项最大输出

    // 输入输出
    float target;            // 目标值
    float feedback;          // 反馈值
    float output;            // 输出值

    // 内部状态
    float p_term;           // 比例项输出
    float i_term;           // 积分项输出
    float d_term;           // 微分项输出
    float error[3];          // 误差缓冲区 (当前,上次,上上次)
}pid_t;

void pidInit(pid_t *pid, float kp, float ki, float kd, float max_output, float max_output_i);
void pidReset(pid_t *pid);
void pidCascadeReset(pid_t *angle_pid, pid_t *speed_pid);
float pidAngle(pid_t *pid, float target, float feedback);
float pidSpeed(pid_t *pid, float target, float feedback);
float pidCascade(pid_t *angle_pid, pid_t *speed_pid,
                 float angle_target,   float angle_feedback,
                 float speed_feedback);
void pidOverZero_8192(const float *target, float *feedback);

#endif //PID_H