//
// Created by DingYong on 2026/5/18.
//

#include "robot_status.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "usart.h"

robot_status_t robot_status;

uint8_t dma_buf[BUF_SIZE];
uint8_t rec_buf[BUF_SIZE];
uint8_t rec_len = 0;

void robot_status_init(void) {
    // 打开串口1的空闲中断
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    // 启动DMA接收:
    // 一直往dma_buf中写,写满128字节才会停止
    HAL_UART_Receive_DMA(&huart1, dma_buf, BUF_SIZE);
}

void robot_status_process(void) {
    if (rec_len < 10) return;

    uint8_t *p = NULL;
    for (int i = 0; i < rec_len - 10; i++) {
        if (rec_buf[i] == 0xA5) {
            p = &rec_buf[i];
            break;
        }
    }

    if (p == NULL) return;

    uint16_t cmd_id = (p[6] << 8) | p[5];

    if (cmd_id == 0x0201) {
        robot_status.cool_rate = (p[14] << 8) | p[13];
        robot_status.heat_max = (p[16] << 8) | p[15];
    }

    if (cmd_id == 0x0202) {
        robot_status.heat_now = (p[18] << 8) | p[17];
    }
}