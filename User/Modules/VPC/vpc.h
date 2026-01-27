//
// Created by DingYong on 25-8-17.
//

#ifndef VPC_H
#define VPC_H

#include <stdint.h>

// 协议常量
#define FRAME_LEN       32     // 帧总长度固定为32字节
#define FRAME_HEADER    's'    // 帧头
#define FRAME_TRAILER   'e'    // 帧尾
#define CMD_A0          0xA0   // 上位机->下位机: 自瞄云台控制
#define CMD_B0          0xB0   // 下位机->上位机: 自瞄数据反馈

// 强制1字节(8bit)对齐(关键! 避免结构体成员偏移错误)
#pragma pack(1)

// 上位机发送给下位机的帧 (0xA0)
typedef struct {
    char start;                // 0: 帧头 's'
    char type;                 // 1: 消息类型 0xA0
    char find_bool;            // 2: 是否追踪 (0/1)
    float yaw;                 // 3-6: 偏航角(4byte)
    float pitch;               // 7_10: 俯仰角(4byte)
    char reserve[20];          // 11-30: 预留空位(20字节)
    char end;                  // 31: 帧尾 'e'
} A0_Frame;

// 下位机发送给上位机的帧 (0xB0)
typedef struct {
    char start;                // 0: 帧头 's'
    char type;                 // 1: 消息类型 0xB0
    float yaw;                 // 2-5: 偏航角 (4字节)
    float pitch;               // 6-9: 俯仰角 (4字节)
    uint8_t enemy_team_color;  // 10: 敌方颜色 (0红 / 1蓝)
    uint8_t mode;              // 11: 模式(0自瞄 / 1符)
    uint8_t rune_flag;         // 12: 符模式(0不可激活/1小符/2大符)
    char reserve[18];          // 13-30: 预留空位(18字节)
    char end;                  // 31: 帧尾 'e'
} B0_Frame;

#pragma pack()      // 恢复默认对齐

void send_b0_frame(float yaw, float pitch, uint8_t color, uint8_t mode, uint8_t rune);

#endif //VPC_H
