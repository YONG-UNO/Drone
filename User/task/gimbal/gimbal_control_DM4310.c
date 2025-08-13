//
// Created by DingYong on 25-8-12.
//

#include "gimbal_control_DM4310.h"

#include "bsp_can.h"
#include "cmsis_os.h"

void gimbalControl_DM4310(void const * argument) {
    for (;;) {
        // 电机使能
        if        (RC.s2 == 3)                 DM4310_Enable();
        else if   (RC.s2 == 1 || RC.s2 == 2)   DM4310_Disable();

        i = pidAngle(&angle_pid_DM4310,(RC.ch1 + 660) / 1320.0f * 8191,motor_measure_DM4310[0].position);
        sendCmdGimbal_DM4310(i);
        osDelay(1);
    }
}
