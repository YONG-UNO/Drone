//
// Created by DingYong on 2025/10/4.
//

#ifndef DRONE_GIMBAL_CONTROL_RS05_H
#define DRONE_GIMBAL_CONTROL_RS05_H
#include "RS05.h"
#include "dbus.h"
#include "usbd_cdc_if.h"
#include "PID/pid.h"

extern float input_DM4310;

extern RC_t RC;
extern pid_t speed_pid_CAN_RS05;
extern pid_t angle_pid_CAN_RS05;
extern motor_measure_RS05_t   motor_measure_RS05[1];
extern aim_receive_decode_t aim_receive_decode;
void gimbalControl_DM4310(void const * argument);

#endif //DRONE_GIMBAL_CONTROL_RS05_H