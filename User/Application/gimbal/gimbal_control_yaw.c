//
// Created by DingYong on 25-8-12.
//

#include "gimbal_control_yaw.h"

#include "bsp_can.h"
#include "cmsis_os.h"
#include "control_switch.h"

//               左限位     中      右限位
//               600      1300     2000
//rad:           4.23     4.7      5.17
//dbus(RC.ch0):  -660      0        660

// 摇杆增量速度
#define MANUAL_SPEED_0 0.01f
#define MANUAL_SPEED_1 0.1f
#define MANUAL_SPEED_1_SLOW 0.01f

static float target_angel_compute(int16_t dbus);
static float ecd_to_rad(uint16_t ecd);

float offset_yaw   = 0.0f;

float target_torque = 0.0f;
float target_angle = YAW_MID;
float manual_angle_6020 = 4.7f;

void gimbalControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {

        if (control_mode == 0) {
            // manufacture enable
            if (RC.s2 == 1) {

                // 摇杆->速度
                float speed = (float)RC.ch0 * MANUAL_SPEED_0;
                // 角度 += 速度 * 时间(1ms)
                target_angle += speed * 0.0001f;
                // 限幅保护
                if (target_angle > YAW_MAX) target_angle = YAW_MAX;
                if (target_angle < YAW_MIN) target_angle = YAW_MIN;


                target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                           &speed_pid_CAN_6020_M4_ID,
                                           target_angle,
                                           ecd_to_rad(motor_measure[3].ecd),
                                           motor_measure[3].speed_rpm);

                //target_angle = manual_angle_6020;
                sendCmdGimbal((int16_t)target_torque);
            }

            // auto aim
            if (RC.s2 == 2) {

                target_angle = aim_receive_use.target_yaw_aim;



                target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                           &speed_pid_CAN_6020_M4_ID,
                                           target_angle,
                                           ecd_to_rad(motor_measure[3].ecd),
                                           motor_measure[3].speed_rpm);

                sendCmdGimbal((int16_t)target_torque);
            }

            // disable
            if (RC.s2 == 3) {


                target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                           &speed_pid_CAN_6020_M4_ID,
                                           target_angle,
                                           ecd_to_rad(motor_measure[3].ecd),
                                           motor_measure[3].speed_rpm);

                sendCmdGimbal((int16_t)0.0f);
            }



        } else if (control_mode == 1) {

            if (gimbal_mode == 0) {
                sendCmdGimbal((int16_t)0.0f);
            } else if (gimbal_mode == 1) {

                if (vt_data.mouse_right == 0) {
                    // 摇杆->速度
                    float speed = (float)vt_data.mouse_x * MANUAL_SPEED_1;

                    if (vt_data.ascii_key == 'C' && vt_data.key == 8196) {
                        target_angle -= 0.00005;
                    }else if (vt_data.ascii_key == 'C' && vt_data.key == 8200) {
                        target_angle += 0.00005;
                    }


                    // 角度 += 速度 * 时间(1ms)
                    target_angle += speed * 0.0001f;
                    // 限幅保护
                    if (target_angle > YAW_MAX) target_angle = YAW_MAX;
                    if (target_angle < YAW_MIN) target_angle = YAW_MIN;


                    target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                               &speed_pid_CAN_6020_M4_ID,
                                               target_angle,
                                               ecd_to_rad(motor_measure[3].ecd),
                                               motor_measure[3].speed_rpm);

                    sendCmdGimbal((int16_t)target_torque);
                }

                if (vt_data.mouse_right == 1) {

                    target_angle = aim_receive_use.target_yaw_aim + offset_yaw;

                    if ((vt_data.ascii_key == 'X') && (vt_data.key == 4128)) {
                        offset_yaw += 0.0001;
                    }else if ((vt_data.ascii_key == 'C') && (vt_data.key == 8224)) {
                        offset_yaw -= 0.0001;
                    }

                    target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                               &speed_pid_CAN_6020_M4_ID,
                                               target_angle,
                                               ecd_to_rad(motor_measure[3].ecd),
                                               motor_measure[3].speed_rpm);

                    sendCmdGimbal((int16_t)target_torque);
                }
            }


        }





        osDelay(1);
    }
}

// 从±660 to 电机弧度
static float target_angel_compute(int16_t dbus) {
    float target_angle = (float)(dbus + 660) / 1320.0f * (5.17f - 4.23f) + 4.23f;
    return target_angle;
}

// 从 0-8191 to 电机弧度
static float ecd_to_rad(uint16_t ecd) {
    float rad = ecd / 8192.0f * 2 * 3.14159265f;
    return rad;
}