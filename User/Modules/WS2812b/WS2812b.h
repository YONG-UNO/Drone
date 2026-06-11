//
// Created by DingYong on 2026/5/17.
//

#ifndef DRONE_WS2812B_H
#define DRONE_WS2812B_H

#include <stdint.h>

#define WS_PIN_PORT     GPIOE
#define WS_PIN          GPIO_PIN_9

#define LED_NUM 3

void ws2812b_r_set(uint8_t r, uint8_t g, uint8_t b);
void ws2812b_l_set(uint8_t r, uint8_t g, uint8_t b);

void ws2812b_r_on(void);
void ws2812b_l_on(void);

#endif //DRONE_WS2812B_H
