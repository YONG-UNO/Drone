//
// Created by DingYong on 25-8-10.
//

#ifndef BSP_IIC_H
#define BSP_IIC_H

#define IST8310_IIC_ADDRESS 0x0E
#include <stdint.h>

void ist8310GpioInit(void);   // 初始化ist8310的gpio(cubeMX已经配好)
void ist8310ComInit(void);    // 初始化ist8310的iic设置(cubeMX已经配好)

uint8_t ist8310ReadRegisterSingle(uint8_t MemAddress );  // 通过iic读取ist8310的一个数据
void ist8310ReadRegisterMultiple(uint8_t MemAddress, uint8_t *buf, uint8_t len);  // 通过iic读取ist8310的多个数据

void ist8310WriteRegisterSingle(uint8_t MemAddress, uint8_t data);  // 通过iic向ist8310写入一个数据
void ist8310WriteRegisterMultiple(uint8_t MemAddress, uint8_t *data, uint8_t len);  // 通过iic向ist8310写入多个数据

void ist8310_RST_H (void);  // 将ist8310的RSTN引脚拉高(1)
void ist8310_RST_L (void);  // 将ist8310的RSTN引脚拉低(0)

void HAL_Delay_us(uint16_t us);  // 使用轮询方式实现us延迟

#endif //BSP_IIC_H
