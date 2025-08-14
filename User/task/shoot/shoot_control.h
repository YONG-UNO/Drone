//
// Created by DingYong on 25-8-13.
//

#ifndef SHOOT_CONTROL_H
#define SHOOT_CONTROL_H
#include "PID.h"

extern float target_rpm_M1;
extern float target_rpm_M2;
extern float target_rpm_M3;

extern pid_t speed_pid_CAN_3508_M1_ID;
extern pid_t speed_pid_CAN_3508_M2_ID;
extern pid_t speed_pid_CAN_2006_M3_ID;

void shootControl(void const * argument);

#endif //SHOOT_CONTROL_H
