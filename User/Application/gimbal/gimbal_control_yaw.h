//
// Created by DingYong on 25-8-12.
//

#ifndef GIMBAL_CONTROL_H
#define GIMBAL_CONTROL_H
#include "dbus.h"
#include "usbd_cdc_if.h"
#include "pid.h"


#define YAW_MAX 5.17
#define YAW_MID 4.7
#define YAW_MIN 4.23

extern RC_t RC;

extern float temp_target_6020;

extern pid_t speed_pid_CAN_6020_M4_ID;
extern pid_t angle_pid_CAN_6020_M4_ID;
extern aim_receive_use_t aim_receive_use;
extern aim_receive_t aim_receive;

void gimbalControl(void const * argument);

#endif //GIMBAL_CONTROL_H
