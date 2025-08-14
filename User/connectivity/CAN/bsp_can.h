//
// Created by DingYong on 25-8-7.
//

#ifndef MYCAN_H
#define MYCAN_H

#include "stm32f4xx_hal.h"

#include "stm32f4xx_hal_can.h"

// CAN1:
# define  CAN_3508_M1_ID    0x201
# define  CAN_3508_M2_ID    0x202
# define  CAN_2006_M3_ID    0x203

// CAN2:
# define  CAN_6020_M4_ID    0x205    //204+1
# define  CAN_4310_M5_ID    0x11

// dm4310配置
# define  PMAX              12.5     // 位置
# define  VMAX              45       // 速度
# define  TMAX              18       // 扭矩


typedef struct {
    uint16_t  ecd;        //6020:机械角[0-8191]
    uint16_t  last_ecd;
    int16_t  speed_rpm;
    int16_t  torque;
    int16_t  given_current;
    uint8_t   temperature;
} motor_measure_t;

typedef struct {
    int p_int;
    int v_int;
    int t_int;
    float position;
    float velocity;
    float torque;
} motor_measure_DM4310_t;


extern motor_measure_t           motor_measure[4];
extern motor_measure_DM4310_t    motor_measure_DM4310[1];

void CAN_Filter_Init(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
void get_motor_measure(motor_measure_t *motor_measure,uint32_t StdId, uint8_t rx_data[]);
void get_motor_measure_DM4310(motor_measure_DM4310_t *motor_measure_DM4310,uint32_t StdId, uint8_t rx_data[]);

void sendCmdShoot(int16_t frictionWheel_l, int16_t frictionWheel_r, int16_t dial);
void sendCmdGimbal(int16_t yaw);
void sendCmdGimbal_DM4310(float torq);

void DM4310_Enable(void);
void DM4310_Disable(void);

#endif //MYCAN_H
