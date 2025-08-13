//
// Created by DingYong on 25-8-12.
//

#ifndef GIMBAL_CONTROL_H
#define GIMBAL_CONTROL_H
#include "dbus.h"
#include "PID.h"

extern float i;

extern RC_t RC;
extern pid_t angle_pid_DM4310;
extern pid_t speed_pid_DM4310;

void gimbalControl_DM4310(void const * argument);

#endif //GIMBAL_CONTROL_H
