//
// Created by DingYong on 25-8-12.
//

#include "gimbal_control.h"

#include "cmsis_os.h"

void gimbalControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {
    //
    //     if (RC.s2 == 1) {
    //         int16_t input_6020 = pidCascade(&angle_pid_CAN_6020_M4_ID, &speed_pid_CAN_6020_M4_ID,
    //                                         temp_target_6020, motor_measure[3].ecd,motor_measure[3].speed_rpm); //5470.0f + RC.ch0/660.0f * 780.0f  6250-4690
    //         sendCmdGimbal(input_6020); // input_6020
    //     }else if (RC.s2 == 2 || RC.s2 == 3) {
    //         int16_t input_6020 = pidCascade(&angle_pid_CAN_6020_M4_ID, &speed_pid_CAN_6020_M4_ID,
    //                                         temp_target_6020, motor_measure[3].ecd,motor_measure[3].speed_rpm);
    //         sendCmdGimbal(0);
    //     }
    //
    osDelay(1);
    }
}
