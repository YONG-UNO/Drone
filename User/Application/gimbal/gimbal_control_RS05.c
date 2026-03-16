//
// Created by DingYong on 2025/10/4.
//

#include "gimbal_control_RS05.h"

#include "bsp_can.h"
#include "cmsis_os.h"

//               上限位     中      下限位
//rad:           1.7       2        2.3
//dbus(RC.ch1):  -660      0        660

static float target_angel_compute(int16_t dbus);

void gimbalControl_RS05(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    // 局部变量：存储目标角度、目标速度、目标力矩
    float target_torque = 0.0f;
    static float target_angle;

    // 第一次使能就行

    for (;;) {

        if (RC.s2 == 1) {
            RS05_Enable();

            target_angle = target_angel_compute(RC.ch1);
            RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,-target_torque);
        }else if (RC.s2 == 3) {
            RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,0.0f);
        }else if (RC.s2 == 2) {
            target_angle = aim_receive_decode.target_pitch_aim;
            RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,-target_torque);
        }

        target_torque = pidCascade(&angle_pid_CAN_RS05,
                                   &speed_pid_CAN_RS05,
                                   target_angle,
                                   motor_measure_RS05[0].position,
                                   motor_measure_RS05[0].velocity);

        osDelay(1);
    }
}

// 从±660 to 电机弧度
static float target_angel_compute(int16_t dbus) {
    float target_angle = (float)(dbus + 660) / 1320.0f * (2.3f - 1.7f) + 1.7f;
    return target_angle;
}
