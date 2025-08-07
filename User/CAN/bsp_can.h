//
// Created by DingYong on 25-8-7.
//

#ifndef MYCAN_H
#define MYCAN_H

#include "stm32f4xx_hal.h"

#include "stm32f4xx_hal_can.h"

# define  CAN_3508_M1_ID    0x201
# define  CAN_3508_M2_ID    0x202
# define  CAN_2006_M3_ID    0x203
# define  CAN_6020_M5_ID    0x208


typedef struct {
    uint16_t ecd;        //6020:机械角[0-8191]
    int16_t  speed_rpm;
    int16_t  given_current;
    uint8_t  temperate;
    int16_t  last_ecd;
    uint16_t torque;
} motor_measure_t;

extern motor_measure_t motor_gimbal[8];

void CAN_Filter_Init(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void get_motor_measure(motor_measure_t *motor_measure,uint32_t StdId, uint8_t rx_data[]);

#endif //MYCAN_H
