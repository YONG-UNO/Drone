//
// Created by DingYong on 2025/10/4.
//

#pragma once

#include "GC.hpp"
#include "RS05.h"
#include "dbus.h"
#include "usbd_cdc_if.h"
#include "pid.h"

extern RC_t RC;
extern pid_t speed_pid_CAN_RS05;
extern pid_t angle_pid_CAN_RS05;
extern motor_measure_RS05_t   motor_measure_RS05[1];
extern aim_receive_decode_t aim_receive_decode;

extern GravityCompensator gc;
extern float output_angle;
extern float target_torque;
#ifdef __cplusplus
extern "C" {
#endif

void gimbalControl_RS05(void const * argument);

#ifdef __cplusplus
}
#endif