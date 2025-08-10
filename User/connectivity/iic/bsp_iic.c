//
// Created by DingYong on 25-8-10.
//

#include "bsp_iic.h"

#include "i2c.h"
#include "stm32f4xx_hal_gpio.h"


// 占位接口:用于初始化ist8310相关GPIO(复位引脚RSTN,DTRY等)
void ist8310GpioInit(void) {

}

// 占位接口:用于配置IIC时钟频率,引脚复用等
void ist8310ComInit(void) {

}

/**
 *@brief read a byte of ist8310 by iic
  * @brief  Read an amount of data in blocking mode from a specific memory address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  *         IIC句柄(如已经初始化的IIC外设,如&hi2c3)
  *
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  *         从设备8位地址(7位设备地址左移一位,最低位预留为读写位)
  *
  * @param  MemAddress Internal memory address
  *         目标寄存器地址(ist8310 的 0x03寄存器)
  *
  * @param  MemAddSize Size of internal memory address
  *         寄存器地址长度(8bit: I2C_MEMADD_SIZE_8BIT; 16bit: I2C_MEMADD_SIZE_16BIT)
  *
  * @param  pData Pointer to data buffer
  *         接收数据的缓冲区(用于存储读取到的寄存器数据)
  *
  * @param  Size Amount of data to be sent
  *         要读取的数据长度(字节数)
  *
  * @param  Timeout Timeout duration
  *         超时时间(毫秒, 超过此时间未完成则返回错误)
  *
  * @retval HAL status
  *
  *@retrval value of the register
  */
uint8_t ist8310ReadRegisterSingle(uint8_t MemAddress ) {
    uint8_t result = 0;
    HAL_I2C_Mem_Read(&hi2c3, IST8310_IIC_ADDRESS << 1, MemAddress,I2C_MEMADD_SIZE_8BIT, &result, 1, 10);
    return result;
}

/**
 * @brief 通过iic读取ist8310的多个字节
 * @param MemAddress 寄存器开始地址
 * @param buf 读取数据缓冲区
 * @param len 读取字节总数
 */
void ist8310ReadRegisterMultiple(uint8_t MemAddress, uint8_t *buf, uint8_t len){
    HAL_I2C_Mem_Read(&hi2c3, IST8310_IIC_ADDRESS << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, buf, len, 10);
}

/**
  * @brief  Write an amount of data in blocking mode to a specific memory address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  *         IIC句柄(如已经初始化的IIC外设,如&hi2c3)
  *
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  *         从设备8位地址(7位设备地址左移一位,最低位预留为读写位)
  *
  * @param  MemAddress Internal memory address
  *         目标寄存器地址(ist8310 的 0x03寄存器)
  *
  * @param  MemAddSize Size of internal memory address
  *         寄存器地址长度(8bit: I2C_MEMADD_SIZE_8BIT; 16bit: I2C_MEMADD_SIZE_16BIT)
  *
  * @param  pData Pointer to data buffer
  *         指向待写入数据的缓冲区指针(如存放配置值的数组)
  *
  * @param  Size Amount of data to be sent
  *         要发送的数据字节数(如写入一个字节的配置值,则为1)
  *
  * @param  Timeout Timeout duration
  *         超时时间(ms):若操作超过此时间未完成,函数会返回超时错误
  *
  * @retval HAL status
  */
void ist8310WriteRegisterSingle(uint8_t MemAddress, uint8_t *data) {
    HAL_I2C_Mem_Write(&hi2c3, IST8310_IIC_ADDRESS << 1, I2C_MEMADD_SIZE_8BIT, 1, data, 1, 10);
}

/**
 * @brief 通过iic向ist8310写入多个数据
 * @param MemAddress 寄存器开始地址
 * @param data 写入数据缓冲区
 * @param len  写入数据长度
 */
void ist8310WriteRegisterMultiple(uint8_t MemAddress, uint8_t *data, uint8_t len) {
    HAL_I2C_Mem_Write(&hi2c3, IST8310_IIC_ADDRESS << 1, I2C_MEMADD_SIZE_8BIT, 1, data, len, 10);
}

/**
  * @brief  设置IST8310磁力计的复位引脚状态
  * @param  PinState: 引脚状态，可取值GPIO_PIN_SET（高电平）或GPIO_PIN_RESET（低电平）
  * @note   复位引脚连接到GPIOG的PIN6
  * @retval None
  */
void ist8310_RST_H (void) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
}

void ist8310_RST_L (void) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
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

