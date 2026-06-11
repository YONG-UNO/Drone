//
// Created by DingYong on 2026/5/13.
//

#ifndef DRONE_VIDEO_TRANSFER_H
#define DRONE_VIDEO_TRANSFER_H

#include <stdint.h>
#include <stdbool.h>

#define VT_UART_HANDEL &huart6
#define VT_FRAME_LEN  21
#define VT_HEAD1      0xA9
#define VT_HEAD2      0x53

#pragma pack(push,1)
typedef struct
{
    uint8_t sof_1;
    uint8_t sof_2;

    uint64_t ch0:11;
    uint64_t ch1:11;
    uint64_t ch2:11;
    uint64_t ch3:11;
    uint64_t mode_sw:2;
    uint64_t pause:1;
    uint64_t fn1:1;
    uint64_t fn2:1;
    uint64_t wheel:11;
    uint64_t trigger:1;

    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;

    uint8_t mouse_left:2;
    uint8_t mouse_right:2;
    uint8_t mouse_middle:2;

    uint16_t key;
    uint16_t crc16;
}vt_raw_t;

// 解析后的数据
typedef struct {
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;

    uint8_t mode_sw;

    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;

    uint8_t mouse_left:2;
    uint8_t mouse_right:2;
    uint8_t mouse_middle:2;

    uint16_t key;
    char ascii_key;

}vt_t;
#pragma pack(pop)

// 全局 接收缓冲区
extern uint8_t vt_rx_buf[VT_FRAME_LEN];
// 全局 解析结果
extern vt_t vt_data;
// 全局 图传状态
extern bool vt_switch;

// 对外函数
void vt_init(void);
void vt_decode(uint8_t *buf, vt_t *out);

#endif //DRONE_VIDEO_TRANSFER_H
