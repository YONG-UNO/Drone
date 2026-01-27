//
// Created by DingYong on 2025/10/4.
//

#include "gimbal_control_RS05.h"

#include "bsp_can.h"
#include "cmsis_os.h"

/**
 * @todo 清零
 * @param rc_value
 * @return
 */
// float RC2Angle_RS05(int16_t rc_value);

void gimbalControl_RS05(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    // 局部变量：存储目标角度、目标速度、目标力矩
    float target_torque = 0.0f;
    static float target_angle = -0.3f;

    for (;;) {
        if (RC.s2 == 1) {
            RS05_Enable();  // 使能电机
        } else if (RC.s2 == 2 || RC.s2 == 3) {
            RS05_Disable(); // 失能电机
        }

        // ************************ 2. 仅在电机使能时执行串级PID控制 ************************
        // a. 摇杆值映射为目标角度（使用RC.ch1，可根据需求改为ch2）
        // target_angle = RC2Angle_RS05(RC.ch0);

        // b. 外环：角度PID计算（输入：目标角度 & 电机实际角度，输出：目标速度）
        target_torque = pidCascade(&angle_pid_CAN_RS05, &speed_pid_CAN_RS05, target_angle, motor_measure_RS05[0].position, motor_measure_RS05[0].velocity);

        // d. 驱动RS05电机（MTT模式：传入目标角度、目标速度、KP、KD、目标力矩）
        // 注：KP/KD使用宏定义值（可根据需求改为动态调整，此处用默认值100.0f和1.0f）
        RS05_MIT_Control(
            0.0f,  // 目标位置（rad）
            0.0f,  // 目标速度（rad/s）
            0.0f,        // KP（位置环比例系数，可调试）
            0.0f,          // KD（速度环微分系数，可调试）
            -target_torque  // 目标力矩（N·m，来自速度PID输出）
        );


        // ************************ 3. 任务延时（控制PID执行频率，推荐20ms，即50Hz） ************************
        // 注：原1ms延时过短，会导致CAN总线过载，修改为20ms
        osDelay(20);
    }

}
// float RC2Angle_RS05(int16_t rc_value) {
//
//     const float input_min = -660.0f;
//     const float input_max = 660.0f;
//     const float output_min = -3.14f;
//     const float output_max = 3.14f;
//
//     // 限幅,防止摇杆超出 -660~660
//     rc_value = (rc_value < -660) ? -660 : (rc_value > 660) ? 660 : rc_value;
//
//     // 线性映射公式:(输入值 - 输入最小值) / (输入最大值 - 输入最小值) * (输出最大值 - 输出最小值) + 输出最小值
//     float target_angle = (rc_value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min;
//
//     return target_angle;
// }