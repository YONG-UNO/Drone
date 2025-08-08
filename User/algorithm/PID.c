//
// Created by DingYong on 25-8-8.
//
// 后续补充:使用增量式pid 使用枚举enum
使用说明
ozone

#include "PID.h"

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

    pid->p_iterm = 0;
    pid->i_iterm = 0;
    pid->d_iterm = 0;

    pid->error[0] = pid->error[1] = pid->error[2] = 0;
}

float pidSpeed(pid_t *pid, float const target, float const feedback) {
    // 更新error
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = target - feedback;

    pid->target   = target;
    pid->feedback = feedback;

    pid->p_iterm =  pid->kp * pid->error[0];                    // 比例项
    pid->i_iterm += pid->ki * pid->error[0];                    // 积分项
    pid->d_iterm =  pid->kd * (pid->error[0] - pid->error[1]);  // 微分项

    // max_output_i
    pid->i_iterm = setOutLimit(pid->i_iterm, pid->max_output_i, -pid->max_output_i);

    // output
    pid->output = pid->p_iterm + pid->i_iterm + pid->d_iterm;

    // max_output
    pid->output = setOutLimit(pid->output, pid->max_output, -pid->max_output);

    return pid->output;
}


float setOutLimit(float const input,float const output_max,float const output_min) {
    if (input > output_max)           return output_max;
    else if (input < output_min)      return output_min;
    else                              return input;
}