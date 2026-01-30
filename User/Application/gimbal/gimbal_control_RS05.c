//
// Created by DingYong on 2025/10/4.
//

#include "gimbal_control_RS05.h"

#include "bsp_can.h"
#include "cmsis_os.h"

#include <math.h>

// 异常保护参数配置（宏定义更便于后续调试修改）
#define MOTOR_POS_MIN    1.51f   // 电机位置下限
#define MOTOR_POS_MAX    2.03f    // 电机位置上限
#define MAX_TORQUE       2.0f    // 最大允许力矩

/**
 * @todo 清零
 * @param rc_value
 * @return
 */
float RC2Angle_RS05(int16_t rc_value);

void gimbalControl_RS05(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    // 局部变量：存储目标角度、目标速度、目标力矩
    float target_torque = 0.0f;
    static float target_angle = 1.7f;

    for (;;) {
        // ************************ 1. 电机基础使能/失能控制（遥控器S2开关） ************************
        if (RC.s2 == 1) {
            RS05_Enable();  // 使能电机（S2拨到1档，进入可控状态）
        } else if (RC.s2 == 2 || RC.s2 == 3) {
            RS05_Disable(); // 失能电机（S2拨到2/3档，电机锁死）
        }

        // ************************ 2. 无论是否使能，都执行PID计算（满足需求2） ************************
        // a. 摇杆值映射为目标角度（反转映射：660→1.47f，-660→2.0f）
        target_angle = RC2Angle_RS05(RC.ch1);

        // b. 串级PID计算（始终执行，即使电机失能）
        target_torque = pidCascade(&angle_pid_CAN_RS05,
                                   &speed_pid_CAN_RS05,
                                   target_angle,
                                   motor_measure_RS05[0].position,
                                   motor_measure_RS05[0].velocity);

        // ************************ 3. 异常保护逻辑（位置超限 || 力矩超限） ************************
        // if ( (motor_measure_RS05[0].position < MOTOR_POS_MIN || motor_measure_RS05[0].position > MOTOR_POS_MAX) ||
        //      (fabs(target_torque) > MAX_TORQUE) ) {
        //     RS05_Disable();  // 强制失能电机，停止所有控制输出
        //     // 可选：添加异常标记（如存入全局变量，用于地面站上报故障）
        //     // g_gimbal_error_flag = 1;
        //     continue;        // 跳过后续电机驱动逻辑，直接进入下一轮循环
        //      }

        // ************************ 4. 仅在电机使能时，驱动RS05电机（失能时不发送指令） ************************
        if (RC.s2 == 1) {
            RS05_MIT_Control(
                0.0f,  // 目标位置（rad）
                0.0f,  // 目标速度（rad/s）
                0.0f,        // KP（位置环比例系数，可调试）
                0.0f,          // KD（速度环微分系数，可调试）
                -target_torque  // 目标力矩（N·m，来自速度PID输出）
            );
        }

        // ************************ 5. 任务延时（控制PID执行频率，20ms=50Hz） ************************
        osDelay(20);
    }
}

float RC2Angle_RS05(int16_t rc_value) {

    const float input_min = -660.0f;
    const float input_max = 660.0f;
    const float output_min = 1.5f;    // 对应摇杆660
    const float output_max = 2.02f;     // 对应摇杆-660
    const float output_range = output_max - output_min;   // 角度范围差值

    // 限幅,防止摇杆超出 -660~660
    rc_value = (rc_value < -660) ? -660 : (rc_value > 660) ? 660 : rc_value;

    // 线性映射公式:(输入值 - 输入最小值) / (输入最大值 - 输入最小值) * (输出最大值 - 输出最小值) + 输出最小值
    float target_angle = (input_max - rc_value) / (input_max - input_min) * output_range + output_min;

    return target_angle;
}