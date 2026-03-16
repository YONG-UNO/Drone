//
// Created by DingYong on 25-8-12.
//

#ifndef GIMBAL_CONTROL_H
#define GIMBAL_CONTROL_H
#include "dbus.h"
#include "usbd_cdc_if.h"
#include "PID/pid.h"

extern RC_t RC;

extern float temp_target_6020;

extern pid_t speed_pid_CAN_6020_M4_ID;
extern pid_t angle_pid_CAN_6020_M4_ID;
extern aim_receive_decode_t aim_receive_decode;


void gimbalControl(void const * argument);

#endif //GIMBAL_CONTROL_H
