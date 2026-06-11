//
// Created by DingYong on 2025/10/4.
//

#include "gimbal_control_pitch.hpp"
#include "bsp_can.h"
#include "cmsis_os.h"
#include "control_switch.h"

//               上限位     中      下限位
//rad:           0.7       1        1.3
//dbus(RC.ch1):  -660      0        660

static float target_angel_compute(int16_t dbus);
float manual_angle_RS05 = 1;
float offset_pitch = 0.0f;

// 摇杆增量速度
#define MANUAL_SPEED_0 0.01f
#define MANUAL_SPEED_1 0.10f
#define MANUAL_SPEED_1_SLOW 0.01f

void gimbalControl_RS05(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    // 局部变量：存储目标角度、目标速度、目标力矩
    float target_torque = 0.0f;
    static float target_angle = PITCH_MID;

    for (;;) {

        if (control_mode == 0) {
            if (RC.s2 == 1) {
                RS05_Enable();

                // 摇杆->速度
                float speed = -(float)RC.ch1 * MANUAL_SPEED_0;
                // 角度 += 速度 * 时间(1ms)
                target_angle += speed * 0.0001f;
                // 限幅保护
                if (target_angle > PITCH_MAX) target_angle = PITCH_MAX;
                if (target_angle < PITCH_MIN) target_angle = PITCH_MIN;

                //target_angle = manual_angle_RS05;

                // temp_torque = 0.644191504 * temp_angle * temp_angle -0.877929091 * temp_angle -0.00996151008;


                target_torque = pidCascade(&angle_pid_CAN_RS05,
                                           &speed_pid_CAN_RS05,
                                           target_angle,
                                           motor_measure_RS05[0].position,
                                           motor_measure_RS05[0].velocity);

                RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f, -target_torque);
            }else if (RC.s2 == 3) {

                target_torque = pidCascade(&angle_pid_CAN_RS05,
                                           &speed_pid_CAN_RS05,
                                           target_angle,
                                           motor_measure_RS05[0].position,
                                           motor_measure_RS05[0].velocity);

                RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,0.0f);
            }else if (RC.s2 == 2) {
                target_angle = aim_receive_use.target_pitch_aim;

                target_torque = pidCascade(&angle_pid_CAN_RS05,
                                           &speed_pid_CAN_RS05,
                                           target_angle,
                                           motor_measure_RS05[0].position,
                                           motor_measure_RS05[0].velocity);

                RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,-target_torque);
            }

        } else if (control_mode == 1) {


            if (gimbal_mode == 0) {
                RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,0.0f);

            } else if (gimbal_mode == 1) {
                RS05_Enable();

                if (vt_data.mouse_right == 0) {
                    // 摇杆->速度
                    float speed = -(float)vt_data.mouse_y * MANUAL_SPEED_1;

                    if (vt_data.ascii_key == 'C' && vt_data.key == 8193) {
                        target_angle -= 0.00005;
                    }else if (vt_data.ascii_key == 'C' && vt_data.key == 8194) {
                        target_angle += 0.00005;
                    }

                    // 角度 += 速度 * 时间(1ms)
                    target_angle += speed * 0.0001f;
                    // 限幅保护
                    if (target_angle > PITCH_MAX) target_angle = PITCH_MAX;
                    if (target_angle < PITCH_MIN) target_angle = PITCH_MIN;


                    target_torque = pidCascade(&angle_pid_CAN_RS05,
                                           &speed_pid_CAN_RS05,
                                           target_angle,
                                           motor_measure_RS05[0].position,
                                           motor_measure_RS05[0].velocity);

                    RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f, -target_torque);
                }

                if (vt_data.mouse_right == 1) {
                    target_angle = aim_receive_use.target_pitch_aim + offset_pitch;

                    if ((vt_data.ascii_key == 'V') && (vt_data.key == 16416)) {
                        offset_pitch += 0.0001;
                    }else if ((vt_data.ascii_key == 'B') && (vt_data.key == 32800)) {
                        offset_pitch -= 0.0001;
                    }
                    target_torque = pidCascade(&angle_pid_CAN_RS05,
                                               &speed_pid_CAN_RS05,
                                               target_angle,
                                               motor_measure_RS05[0].position,
                                               motor_measure_RS05[0].velocity);

                    RS05_MIT_Control(0.0f, 0.0f, 0.0f,0.0f,-target_torque);
                }
            }


        }

        osDelay(1);
    }
}

// 从±660 to 电机弧度
static float target_angel_compute(int16_t dbus) {
    float target_angle = (float)(dbus + 660) / 1320.0f * (1.3f - 0.7f) + 0.7f;
    return target_angle;
}
