//
// Created by DingYong on 25-8-20.
//

#ifndef SSD1306_H
#define SSD1306_H

#define OLED_I2C_ADDRESS 0x78   // oled设备地址

#define OLED_CMD   0x00   // 代表写入的字节是指令
#define OLED_DATA  0x40   // 代表写入的字节是数据
#include <stdint.h>

extern  uint8_t OLED_GRAM[128][8];

typedef enum  {
    PEN_WRITE     = 0x00,   // 填充0xFF(全屏点亮)
    PEN_CLEAR     = 0x01,   // 填充0x00(清屏)
    PEN_INVERSION = 0x02    // 按位取反(反色显示)
} pen_typedef;

void OLED_init(void);
void OLED_display_ON(void);
void OLED_display_OFF(void);
void OLED_operate_gram(pen_typedef operate);
void OLED_set_position(uint8_t x, uint8_t y);
void OLED_draw_point(uint8_t x, uint8_t y, pen_typedef operate);
void OLED_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, pen_typedef pen);
void OLED_draw_char(uint8_t row, uint8_t column, uint8_t chr);
void OLED_draw_string(uint8_t row, uint8_t column, char *string);
void OLED_refresh_gram(void);

#endif //SSD1306_H
