//
// Created by DingYong on 2025/12/6.
//

#ifndef RS05_H
#define RS05_H

#include <stdint.h>
#include <stdbool.h>

#define RS05_CAN_ID    0x08  // RS05电机CAN ID (MCU->SLAVE)
#define RS05_CAN_HANDLE &hcan2
#define RS05_MASTER_ID 0x12  // RS05电机MASTER ID (SLAVE->MCU)
#define P_MIN (-12.57f)  // 添加f表示这是float,如果不加编译器默认为double
#define P_MAX   12.57f
#define V_MIN (-50.0f)
#define V_MAX   50.0f
#define KP_MIN  0.0f
#define KP_MAX  500.0f
#define KD_MIN  0.0f
#define KD_MAX  5.0f
#define T_MIN (-5.5f)
#define T_MAX   5.5f

typedef struct {
    int p_int;  // 位置映射整数（协议：0~65535 → 实际：-12.57~12.57rad）
    int v_int;  // 速度映射整数（协议：0~4096 → 实际：-50~50rad/s）
    int t_int;  // 力矩映射整数（协议：0~4096 → 实际：-5.5~5.5N·m）
    float position;  // 实际位置
    float velocity;  // 实际速度
    float torque;    // 实际力矩
} motor_measure_RS05_t;


#ifdef __cplusplus
extern "C" {
#endif

// RS05 MTT协议打包接口（仅打包，不发送，解耦业务和底层）
void RS05_MIT_Pack(float target_pos, float target_vel, float kp, float kd, float target_torque, uint8_t mtt_tx_data[8]);

// MTT控制接口(业务逻辑入口, 依赖通用CAN发送接口)
bool RS05_MIT_Control(float target_pos, float target_vel, float kp, float kd, float target_torque);

// 反馈解析接口
void get_motor_measure_RS05(motor_measure_RS05_t *motor_measure_RS05,uint32_t StdId, const uint8_t rx_data[]);

// 电机使能接口
bool RS05_Enable(void);
// 电机失能接口
bool RS05_Disable(void);
// 电机调试接口
bool RS05_Debug(void);

#ifdef __cplusplus
 }
#endif

#endif //RS05_H

