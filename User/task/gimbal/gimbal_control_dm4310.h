//
// Created by DingYong on 2025/10/4.
//

#ifndef DRONE_GIMBAL_CONTROL_DM4310_H
#define DRONE_GIMBAL_CONTROL_DM4310_H
#include "dbus.h"
#include "PID.h"

extern float input_DM4310;

extern RC_t RC;
extern pid_t speed_pid_DM4310;
extern pid_t angle_pid_DM4310;

void gimbalControl_DM4310(void const * argument);

#endif //DRONE_GIMBAL_CONTROL_DM4310_H