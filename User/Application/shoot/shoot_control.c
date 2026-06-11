//
// Created by DingYong on 25-8-13.
//

#include "shoot_control.h"

#include "bsp_can.h"
#include "cmsis_os.h"
#include "dbus.h"
#include "control_switch.h"
#include "robot_status.h"


float target_rpm_M1 = -6500.0f;
float target_rpm_M2 =  6500.0f;
float target_rpm_M3 = 2000.0f;//10800.0f

float target_position = 1024.0f;
float target_torque_2006 = 0.0f;
static int control_cnt = 0;

static int CONTROL_TOTAL = 125; // 等待时间

void shootControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    // 摩擦轮开启标志位
    uint8_t friction_wheel_enabled = 0;

    int16_t input_2006 = 0; // 2006输入电流

    for (;;) {

        int RAMP_FRAME = CONTROL_TOTAL / 2;
        float STEP = (1024.0f / RAMP_FRAME);

        if (control_mode == 0) {
            if (RC.s1 == 1) {
                friction_wheel_enabled = 1;
                int16_t input_3508_l = pidSpeed(&speed_pid_CAN_3508_M1_ID,target_rpm_M1,motor_measure[0].speed_rpm);
                int16_t input_3508_r = pidSpeed(&speed_pid_CAN_3508_M2_ID,target_rpm_M2,motor_measure[1].speed_rpm);
                sendCmdShoot(input_3508_l,input_3508_r,0);  // input_3508_l input_3508_r
            }else if (RC.s1 == 2 || RC.s1 == 3) {
                friction_wheel_enabled = 0;
                if (motor_measure[0].speed_rpm < -10 && motor_measure[1].speed_rpm > 10) {
                    int16_t input_3508_l = pidSpeed(&speed_pid_CAN_3508_M1_ID,0,motor_measure[0].speed_rpm);
                    int16_t input_3508_r = pidSpeed(&speed_pid_CAN_3508_M2_ID,0,motor_measure[1].speed_rpm);
                    sendCmdShoot(input_3508_l,input_3508_r,0);
                }

                sendCmdShoot(0,0,0);
            }

            if (friction_wheel_enabled == 1) { // 当摩擦轮启动时才可以开启拨弹盘

                control_cnt ++;

                if (control_cnt <= RAMP_FRAME) {

                    float next_pos = target_position; // 过零记忆

                    if (RC.dialWheel >= 1024 + 440) {
                        next_pos += STEP;
                    } else if (RC.dialWheel < 700) {
                        next_pos -= STEP;
                    }

                    if (next_pos >= 8191.0f) {
                        target_position = next_pos - 8192.0f;
                    }else if (next_pos < 0.0f) {
                        target_position = next_pos + 8192.0f;
                    }else {
                        target_position = next_pos;
                    }
                }

                if (control_cnt >= CONTROL_TOTAL) {
                    control_cnt = 0;
                }

                if (RC.dialWheel < 1024+440 && RC.dialWheel >= 700) {

                    target_torque_2006 = pidCascade(&angle_pid_CAN_2006_M3_ID,
                                                    &speed_pid_CAN_2006_M3_ID,
                                                             target_position,
                                                motor_measure[2].ecd,
                                               motor_measure[2].speed_rpm);

                    sendCmdShoot(0,0,target_torque_2006);
                } else {

                    target_torque_2006 = pidCascade(&angle_pid_CAN_2006_M3_ID,
                                                    &speed_pid_CAN_2006_M3_ID,
                                                             target_position,
                                                motor_measure[2].ecd,
                                               motor_measure[2].speed_rpm);

                    sendCmdShoot(0,0,target_torque_2006);
                }

            } else { // 摩擦轮未开启,拨弹盘无法输出

                target_torque_2006 = pidCascade(&angle_pid_CAN_2006_M3_ID,
                                                &speed_pid_CAN_2006_M3_ID,
                                                         target_position,
                                            motor_measure[2].ecd,
                                           motor_measure[2].speed_rpm);

                sendCmdShoot(0,0,0);
            }



        } else if (control_mode == 1) {

            if (friction_mode == 1) {
                friction_wheel_enabled = 1;
                int16_t input_3508_l = pidSpeed(&speed_pid_CAN_3508_M1_ID,target_rpm_M1,motor_measure[0].speed_rpm);
                int16_t input_3508_r = pidSpeed(&speed_pid_CAN_3508_M2_ID,target_rpm_M2,motor_measure[1].speed_rpm);
                sendCmdShoot(input_3508_l,input_3508_r,0);  // input_3508_l input_3508_r
            }else if (friction_mode == 0) {
                friction_wheel_enabled = 0;
                if (motor_measure[0].speed_rpm < -10 && motor_measure[1].speed_rpm > 10) {
                    int16_t input_3508_l = pidSpeed(&speed_pid_CAN_3508_M1_ID,0,motor_measure[0].speed_rpm);
                    int16_t input_3508_r = pidSpeed(&speed_pid_CAN_3508_M2_ID,0,motor_measure[1].speed_rpm);
                    sendCmdShoot(input_3508_l,input_3508_r,0);
                }

                sendCmdShoot(0,0,0);
            }

            if (friction_wheel_enabled == 1) { // 当摩擦轮启动时才可以开启拨弹盘

                control_cnt ++;

                if (control_cnt <= RAMP_FRAME) {

                    float next_pos = target_position; // 过零记忆

                    if (vt_data.mouse_left == 1 && vt_data.ascii_key == 'c') {
                        next_pos -= STEP;
                    } else if (vt_data.mouse_left == 1) {
                        next_pos += STEP;
                    }

                    if (next_pos >= 8191.0f) {
                        target_position = next_pos - 8192.0f;
                    }else if (next_pos < 0.0f) {
                        target_position = next_pos + 8192.0f;
                    }else {
                        target_position = next_pos;
                    }
                }

                if (control_cnt >= CONTROL_TOTAL) {
                    control_cnt = 0;
                }

                if (vt_data.mouse_left == 0) {

                    target_torque_2006 = pidCascade(&angle_pid_CAN_2006_M3_ID,
                                                    &speed_pid_CAN_2006_M3_ID,
                                                             target_position,
                                                motor_measure[2].ecd,
                                               motor_measure[2].speed_rpm);

                    sendCmdShoot(0,0,target_torque_2006);
                } else {

                    target_torque_2006 = pidCascade(&angle_pid_CAN_2006_M3_ID,
                                                    &speed_pid_CAN_2006_M3_ID,
                                                             target_position,
                                                motor_measure[2].ecd,
                                               motor_measure[2].speed_rpm);

                    sendCmdShoot(0,0,target_torque_2006);
                }

            } else { // 摩擦轮未开启,拨弹盘无法输出

                target_torque_2006 = pidCascade(&angle_pid_CAN_2006_M3_ID,
                                                &speed_pid_CAN_2006_M3_ID,
                                                         target_position,
                                            motor_measure[2].ecd,
                                           motor_measure[2].speed_rpm);

                sendCmdShoot(0,0,0);
            }

        }



        // 调整摩擦轮速度 F+ G-
        if (vt_data.ascii_key == 'F') {
            target_rpm_M1 -= 1;
            target_rpm_M2 += 1;
        } else if (vt_data.ascii_key == 'G') {
            target_rpm_M1 += 1;
            target_rpm_M2 -= 1;
        }

        robot_status_process();

        osDelay(1);
    }
}