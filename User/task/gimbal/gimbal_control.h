//
// Created by DingYong on 25-8-12.
//

#ifndef GIMBAL_CONTROL_H
#define GIMBAL_CONTROL_H
#include "dbus.h"
#include "PID.h"

extern RC_t RC;

extern pid_t speed_pid_CAN_6020_M4_ID;
extern pid_t angle_pid_CAN_6020_M4_ID;

void gimbalControl(void const * argument);

#endif //GIMBAL_CONTROL_H
