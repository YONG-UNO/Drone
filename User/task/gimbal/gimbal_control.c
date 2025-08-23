//
// Created by DingYong on 25-8-12.
//

#include "gimbal_control.h"

#include "bsp_can.h"
#include "cmsis_os.h"

void gimbalControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {
        // // 电机使能
        // if        (RC.s2 == 3)                 DM4310_Enable();
        // else if   (RC.s2 == 1 || RC.s2 == 2)   DM4310_Disable();
        //
        // input_DM4310 = pidAngle(&angle_pid_DM4310,(RC.ch1 + 660) / 1320.0f * 600 + 4000,motor_measure_DM4310[0].position);
        // sendCmdGimbal_DM4310(0);

        if (RC.s1 == 1) {
            int16_t input_6020 = pidCascade(&angle_pid_CAN_6020_M4_ID, &speed_pid_CAN_6020_M4_ID,
                                            2700.0f + RC.ch0/660.0f * 300.0f, motor_measure[3].ecd,motor_measure[3].speed_rpm);
            sendCmdGimbal(0  );
        }else if (RC.s1 == 2 || RC.s1 == 3) {
            int16_t input_6020 = pidCascade(&angle_pid_CAN_6020_M4_ID, &speed_pid_CAN_6020_M4_ID,
                                            0, motor_measure[3].ecd,motor_measure[3].speed_rpm);
            sendCmdGimbal(0);
        }

        osDelay(1);
    }
}
