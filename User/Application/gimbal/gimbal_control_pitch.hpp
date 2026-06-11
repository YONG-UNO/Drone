//
// Created by DingYong on 2025/10/4.
//

#pragma once

// #include "GC.hpp"
#include "RS05.h"
#include "dbus.h"
#include "usbd_cdc_if.h"
#include "pid.h"


#define PITCH_MAX 1.3
#define PITCH_MID 1
#define PITCH_MIN 0.7

extern RC_t RC;
extern pid_t speed_pid_CAN_RS05;
extern pid_t angle_pid_CAN_RS05;
extern motor_measure_RS05_t   motor_measure_RS05[1];
extern aim_receive_t aim_receive;
extern aim_receive_use_t aim_receive_use;

// extern GravityCompensator gc;
extern float output_angle;
extern float target_torque;
#ifdef __cplusplus
extern "C" {
#endif

void gimbalControl_RS05(void const * argument);

#ifdef __cplusplus
}
#endif