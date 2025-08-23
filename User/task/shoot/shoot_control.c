//
// Created by DingYong on 25-8-13.
//

#include "shoot_control.h"

#include "bsp_can.h"
#include "cmsis_os.h"
#include "User/task/gimbal/gimbal_control.h"

void shootControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {
        if (RC.s1 == 1) {
            int16_t input_3508_l = pidSpeed(&speed_pid_CAN_3508_M1_ID,target_rpm_M1,motor_measure[0].speed_rpm);
            int16_t input_3508_r = pidSpeed(&speed_pid_CAN_3508_M2_ID,target_rpm_M2,motor_measure[1].speed_rpm);
            sendCmdShoot(0,0,0);
        }else if (RC.s1 == 2 || RC.s1 == 3) {
            float input_3508_l = pidSpeed(&speed_pid_CAN_3508_M1_ID,0,motor_measure[0].speed_rpm);
            float input_3508_r = pidSpeed(&speed_pid_CAN_3508_M2_ID,0,motor_measure[1].speed_rpm);
            sendCmdShoot(0,0,0);
        }

        if (RC.dialWheel >= 1024 + 440) {
            float input_2006 = pidSpeed(&speed_pid_CAN_2006_M3_ID, target_rpm_M3, motor_measure[2].speed_rpm);
            sendCmdShoot(0,0,0); // input_2006
        }else if (RC.dialWheel < 1024+440 && RC.dialWheel >= 0) {
            float input_2006 = pidSpeed(&speed_pid_CAN_2006_M3_ID, target_rpm_M3, motor_measure[2].speed_rpm);
            sendCmdShoot(0,0,0);
        }else {
            float input_2006 = pidSpeed(&speed_pid_CAN_2006_M3_ID, target_rpm_M3, motor_measure[2].speed_rpm);
            sendCmdShoot(0,0,0);
        }



        osDelay(1);
    }
}


