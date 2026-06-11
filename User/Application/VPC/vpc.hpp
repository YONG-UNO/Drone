//
// Created by DingYong on 25-8-17.
//

#ifndef VPC_H
#define VPC_H
#include <stdint.h>

/* typedef struct */
#pragma pack(1)
typedef struct {
    char start;                // 0 帧头，取 's'
    char type;                 // 1 消息类型 0xB0
    float yaw;                 // 2 - 5 偏航角
    float pitch;               // 6 - 9 俯仰角
    uint8_t enemy_team_color;  // 10  敌方颜色 0 ：红 1 ：蓝
    uint8_t mode;              // 11 模式 0 ：自瞄 1 ：符
    uint8_t rune_flag;         // 12 符模式 '0'：不可激活 '1'：小符 '2':大符
    uint8_t reverse[18];       // 13 - 30 预留空位
    char end;                  // 31 帧尾，取 'e'
} aim_transmit_t;
#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

/* function */
void USB(void const * argument);

#ifdef __cplusplus
}
#endif

#endif //VPC_H
