//
// Created by DingYong on 25-8-10.
//

#ifndef BSP_IIC_H
#define BSP_IIC_H

#include <stdint.h>

#define OLED_I2C_ADDRESS 0x78   // oled设备地址

#define OLED_CMD   0x00   // 代表写入的字节是指令
#define OLED_DATA  0x40   // 代表写入的字节是数据

extern  uint8_t OLED_GRAM[128][8];

typedef enum  {
    PEN_WRITE     = 0x00,   // 填充0xFF(全屏点亮)
    PEN_CLEAR     = 0x01,   // 填充0x00(清屏)
    PEN_INVERSION = 0x02    // 按位取反(反色显示)
} pen_typedef;

// #define IST8310_IIC_ADDRESS 0x0E

// void ist8310GpioInit(void);   // 初始化ist8310的gpio(cubeMX已经配好)
// void ist8310ComInit(void);    // 初始化ist8310的iic设置(cubeMX已经配好)
//
// uint8_t ist8310ReadRegisterSingle(uint8_t MemAddress );  // 通过iic读取ist8310的一个数据
// void ist8310ReadRegisterMultiple(uint8_t MemAddress, uint8_t *buf, uint8_t len);  // 通过iic读取ist8310的多个数据
//
// void ist8310WriteRegisterSingle(uint8_t MemAddress, uint8_t data);  // 通过iic向ist8310写入一个数据
// void ist8310WriteRegisterMultiple(uint8_t MemAddress, uint8_t *data, uint8_t len);  // 通过iic向ist8310写入多个数据
//
// void ist8310_RST_H (void);  // 将ist8310的RSTN引脚拉高(1)
// void ist8310_RST_L (void);  // 将ist8310的RSTN引脚拉低(0)
//
// void HAL_Delay_us(uint16_t us);  // 使用轮询方式实现us延迟

void OLED_init(void);
void OLED_display_ON(void);
void OLED_display_OFF(void);
void OLED_operate_gram(pen_typedef operate);
void OLED_set_position(uint8_t x, uint8_t y);
void OLED_draw_point(int8_t x, int8_t y, pen_typedef operate);
void OLED_refresh_gram(void);
#endif //BSP_IIC_H
