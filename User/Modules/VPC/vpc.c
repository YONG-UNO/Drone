//
// Created by DingYong on 25-8-17.
//

#include "vpc.h"

#include "usbd_cdc_if.h"

// 上位机发送A0到下位机在usbd_cdc_if.c

// 发送0xB0反馈帧到上位机
void send_b0_frame(float yaw, float pitch, uint8_t color, uint8_t mide, uint8_t rune) {
    // 1. 初始化B0帧并填充默认值
    B0_Frame b0_frame = {
        .start = FRAME_HEADER,     // 帧头 's'
        .type = CMD_B0,            // 类型0xB0
        .end = FRAME_TRAILER       // 帧尾 'e'
    };

    // 2. 填充有效数据
    b0_frame.yaw = yaw;
    b0_frame.pitch = pitch;
    b0_frame.enemy_team_color = color;
    b0_frame.mode = mide;
    b0_frame.rune_flag = rune;

    // 3. 发送整个帧(固定32字节)
    CDC_Transmit_FS((uint8_t *)&b0_frame, FRAME_LEN);
}

