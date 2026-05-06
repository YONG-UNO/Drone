//
// Created by DingYong on 25-8-17.
//

#include "vpc.hpp"

#include "bsp_can.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"
#include "dbus.h"
#include "gimbal_control_RS05.hpp"

// 接收逻辑写在中断（仅做校验 + 缓存），解析 + 发送回复写在 FreeRTOS 任务，
// 绝对不要把发送回复直接写在中断里！

void USB(void const * argument) {
    (void)argument;

    aim_transmit_t aim_transmit={
        .start = 's',
        .type = 0xB0,
        .yaw = 0.0f,
        .pitch = 0.0f,
        .enemy_team_color = 0, // 默认红方
        .mode = 0, // 默认自瞄模式
        .rune_flag = 0, // 默认不可激活符
        .reverse = {0},
        .end = 'e'
    };

    for (;;) {
        aim_transmit.yaw = (float)motor_measure[3].ecd / 8192.0f * 2 * 3.14159265f;// aim_receive.yaw;
        aim_transmit.pitch = motor_measure_RS05[0].position;// aim_receive.pitch;

        CDC_Transmit_FS((uint8_t *)&aim_transmit,sizeof(aim_transmit));
        osDelay(5);
    }
}