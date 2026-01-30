//
// Created by DingYong on 25-8-12.
//

#include "gimbal_control.h"

#include "bsp_can.h"
#include "cmsis_os.h"

// 异常保护参数配置（宏定义更便于后续调试修改）
#define MOTOR_POS_MIN    1.51f   // 电机位置下限
#define MOTOR_POS_MAX    2.03f    // 电机位置上限
#define MAX_TORQUE       2.0f    // 最大允许力矩

float RC2Angle_DJI6020(int16_t rc_value);

// 局部使能标记位（这里用局部，仅任务内有效）
static uint8_t gimbal_enabled_flag = 0; // 0=失能，1=使能

void gimbalControl(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    // 局部变量：存储目标角度、目标速度、目标力矩
    float target_torque = 0.0f;
    static float target_angle = 1.7f;

    for (;;) {
        // ************************ 1. 电机基础使能/失能控制（遥控器S1开关，统一控制） ************************
        // 【修改3】：梳理S1开关逻辑，设置使能标记位
        if (RC.s1 == 1) {
            // S1=1档：使能云台（初始化+标记使能）
            gimbal_enabled_flag = 1;
            sendCmdGimbal(0.0f); // 【修改4】：修复函数重载，统一用带参调用（0力矩初始化使能，匹配电机协议）
        } else if (RC.s1 == 2 || RC.s1 == 3) {
            // S1=2/3档：失能云台（标记失能+发送0力矩锁死）
            gimbal_enabled_flag = 0;
            sendCmdGimbal(0.0f); // 发送0力矩，让电机锁死/进入空闲状态
            osDelay(1); // 延时后直接进入下一轮，减少无效操作
            continue;
        }

        // ************************ 2. 仅使能状态下，执行核心控制逻辑（可选，也可保留原逻辑始终执行PID） ************************
        if (gimbal_enabled_flag == 1) {
            // a. 摇杆值映射为目标角度（反转映射：660→1.47f，-660→2.0f）
            target_angle = RC2Angle_DJI6020(RC.ch0);

            // b. 串级PID计算（得到目标力矩）
            target_torque = pidCascade(&angle_pid_CAN_6020_M4_ID,
                                       &speed_pid_CAN_6020_M4_ID,
                                       target_angle,
                                       motor_measure[3].ecd,
                                       motor_measure[3].speed_rpm);

            // d. 驱动电机：发送目标力矩指令
            sendCmdGimbal(0);
        } else {
            // 失能状态下，强制发送0力矩（双重保       障）
            sendCmdGimbal(0.0f);
        }

        // ************************ 3. 任务延时（控制PID执行频率，20ms=50Hz） ************************
        osDelay(1);
    }
}

float RC2Angle_DJI6020(int16_t rc_value) {

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
