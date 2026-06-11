//
// Created by DingYong on 2026/5/17.
//

#include "WS2812b.h"

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

#define CODE_0   14
#define CODE_1   56
#define RESET_SLOTS   40

uint8_t s_r_r, s_r_g, s_r_b;
uint8_t s_l_r, s_l_g, s_l_b;

static uint16_t pwmBuf_r[LED_NUM * 24 + RESET_SLOTS];
static uint16_t pwmBuf_l[LED_NUM * 24 + RESET_SLOTS];

void ws2812b_r_set(uint8_t r, uint8_t g, uint8_t b) {
    s_r_r = r;
    s_r_g = g;
    s_r_b = b;
}

void ws2812b_l_set(uint8_t r, uint8_t g, uint8_t b) {
    s_l_r = r;
    s_l_g = g;
    s_l_b = b;
}

void ws2812b_r_on(void)
{
    uint32_t idx = 0;

    // 等待DMA空闲
    while (HAL_DMA_GetState(htim1.hdma[TIM_DMA_ID_CC1]) != HAL_DMA_STATE_READY);
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);

    // 填充 GRB 数据（高位先发）
    for(int led = 0; led < LED_NUM; led++)
    {
        // G
        for(int i = 7; i >= 0; i--)
        {
            pwmBuf_r[idx++] = (s_r_g & (1 << i)) ? CODE_1 : CODE_0;
        }
        // R
        for(int i = 7; i >= 0; i--)
        {
            pwmBuf_r[idx++] = (s_r_r & (1 << i)) ? CODE_1 : CODE_0;
        }
        // B
        for(int i = 7; i >= 0; i--)
        {
            pwmBuf_r[idx++] = (s_r_b & (1 << i)) ? CODE_1 : CODE_0;
        }
    }

    // 复位码
    for(int i = 0; i < RESET_SLOTS; i++)
    {
        pwmBuf_r[idx++] = 0;
    }

    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmBuf_r, idx);
}

void ws2812b_l_on(void)
{
    uint32_t idx = 0;

    // 等待DMA空闲
    while (HAL_DMA_GetState(htim1.hdma[TIM_DMA_ID_CC2]) != HAL_DMA_STATE_READY);
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_2);

    // 填充 GRB 数据（高位先发）
    for(int led = 0; led < LED_NUM; led++)
    {
        // G
        for(int i = 7; i >= 0; i--)
        {
            pwmBuf_l[idx++] = (s_l_g & (1 << i)) ? CODE_1 : CODE_0;
        }
        // R
        for(int i = 7; i >= 0; i--)
        {
            pwmBuf_l[idx++] = (s_l_r & (1 << i)) ? CODE_1 : CODE_0;
        }
        // B
        for(int i = 7; i >= 0; i--)
        {
            pwmBuf_l[idx++] = (s_l_b & (1 << i)) ? CODE_1 : CODE_0;
        }
    }

    // 复位码
    for(int i = 0; i < RESET_SLOTS; i++)
    {
        pwmBuf_l[idx++] = 0;
    }

    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t *)pwmBuf_l, idx);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
        HAL_TIM_PWM_Stop_DMA(&htim1,TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_DMA(&htim1,TIM_CHANNEL_2);
    }
}