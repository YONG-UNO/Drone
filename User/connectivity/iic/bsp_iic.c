//
// Created by DingYong on 25-8-10.
//

#include "bsp_iic.h"

#include "stm32f4xx_hal_gpio.h"

/**
  * @brief  设置IST8310磁力计的复位引脚状态
  * @param  PinState: 引脚状态，可取值GPIO_PIN_SET（高电平）或GPIO_PIN_RESET（低电平）
  * @note   复位引脚连接到GPIOG的PIN6
  * @retval None
  */
void ist8310ResetStatus(GPIO_PinState pinState) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, pinState);
}


/**
 * @brief us级延迟(轮询实现)
 * @param us
 * @fixme 用中断实现
 * @todo 重复写
 */
void HAL_Delay_us(uint16_t us)
{
    uint32_t ticks = 0;
    uint32_t told = 0, tnow = 0, tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;       // 获取SysTick最大重装载值(二十四位递减器: 2^24 - 1 = 16777215) (计数到0后会从从该值开始递减)
    ticks = us * 72;              // 假设系统时钟为72MHz(1e6 us = 72e6 时钟周期 | 1us = 72时钟周期)
    told = SysTick->VAL;          // 实时存储当前计数器的数值(递减过程中不断变化)
    while (1)
    {
        tnow = SysTick->VAL;      // 获取当前计数值
        if (tnow != told){        // 计数器值发生变化时才计数

            /*判断讲解
             *举例:reload = 100(大概是7位减数器:128) 系统时钟1MHz(1us = 1计数),即ticks = us * 1
             *目标延迟 50us
             *
             *情况1:未溢出
             *初始时刻:told = 80(记录初始计数)
             *第一次循环:tnow = 60
             *判断:tnow(60) < told(80) -> 未溢出
             *累计计数:tcnt += 80 - 60 = 20
             *第二次循环:tnow = 30(计数器又减了30)
             *判断:tnow(30) < told(60) -> 未溢出
             *累计计数:tcnt += 60 - 30 -> tcnt = 20 + 30 = 50 >= ticks(50) -> break
             *
             *情况2:溢出(tnow > told)
             *初始时刻:told = 20(记录初始计数)
             *第一次循环:tnow = 90 (计数器从20减到0后,重装载reload = 100,再减到90)
             *判断:tnow(90) > told(20) ->溢出
             *累计计数:从told(20)到0的差 + 从reload(100)到tnow(90)的差 = 20 + 10 = 30
             *第二次循环:tnow = 70(计数器经过一次while从90到70)
             *判断:tnow(70) < told(90) -> 未溢出
             *累计计数:tcnt += 70 - 90 -> tcnt = 30 + 20 = 50 >= ticks(50) -> break
             */

            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += (reload - tnow) + (told - 0);
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

