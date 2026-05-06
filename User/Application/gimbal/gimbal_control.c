//
// Created by DingYong on 25-8-12.
//

#include "gimbal_control.h"

#include "bsp_can.h"
#include "cmsis_os.h"

//               左限位     中      右限位
//               600      1300     2000
//rad:           0.9970  1.5340   2.071
//dbus(RC.ch0):  -660      0        660

static float target_angel_compute(int16_t dbus);
static float ecd_to_rad(uint16_t ecd);

float target_torque = 0.0f;
float target_angle = 0.9971f;

void gimbalControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {

        target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                   &speed_pid_CAN_6020_M4_ID,
                                   target_angle,
                                   ecd_to_rad(motor_measure[3].ecd),
                                   motor_measure[3].speed_rpm);

        // manufacture enable
        if (RC.s2 == 1) {
            target_angle = target_angel_compute(RC.ch0);
            sendCmdGimbal((int16_t)0.0f);
        }

        // auto aim
        if (RC.s2 == 2) {
            target_angle = aim_receive_decode.target_yaw_aim;
            sendCmdGimbal((int16_t)target_torque);
        }

        // disable
        if (RC.s2 == 3) {
            target_angle = target_angel_compute(RC.ch0);
            sendCmdGimbal((int16_t)0.0f);
        }

        osDelay(1);
    }
}

// 从±660 to 电机弧度
static float target_angel_compute(int16_t dbus) {
    float target_angle = (float)(dbus + 660) / 1320.0f * (2.071f - 0.9970f) + 0.9970;
    return target_angle;
}

// 从 0-8191 to 电机弧度
static float ecd_to_rad(uint16_t ecd) {
    float rad = ecd / 8192.0f * 2 * 3.14159265f;
    return rad;
}