//
// Created by DingYong on 2025/10/4.
//

#include "gimbal_control_dm4310.h"

#include "bsp_can.h"
#include "cmsis_os.h"

void gimbalControl_DM4310(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {
        // 电机使能
        if        (RC.s2 == 1)                 DM4310_Enable();
        else if   (RC.s2 == 2 || RC.s2 == 3)   DM4310_Disable();

        input_DM4310 = pidAngle(&angle_pid_DM4310,(RC.ch1 + 660) / 1320.0f * 600 + 4000,motor_measure_DM4310[0].position);
        sendCmdGimbal_DM4310(input_DM4310);

        osDelay(1);
    }
}