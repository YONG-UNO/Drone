//
// Created by DingYong on 25-8-5.
//

#ifndef DBUS_H
#define DBUS_H
#include <stdint.h>


//按键掩码
#define KEY_MASK_W               ((uint16_t)0x01 << 0)
#define KEY_MASK_S               ((uint16_t)0x01 << 1)
#define KEY_MASK_A               ((uint16_t)0x01 << 2)
#define KEY_MASK_D               ((uint16_t)0x01 << 3)
#define KEY_MASK_Q               ((uint16_t)0x01 << 4)
#define KEY_MASK_E               ((uint16_t)0x01 << 5)
#define KEY_MASK_SHIFT           ((uint16_t)0x01 << 6)
#define KEY_MASK_CTRL            ((uint16_t)0x01 << 7)

//按键检测宏(检测按键状态)
//若对应按键被按下,则与对应按键掩码相与后逻辑为1->按下,否则为0->未按下
#define KEY_PRESSED_W            ((RC.key.all & KEY_MASK_W) != 0)   //&优先级低于!=
#define KEY_PRESSED_S            ((RC.key.all & KEY_MASK_S) != 0)
#define KEY_PRESSED_A            ((RC.key.all & KEY_MASK_A) != 0)
#define KEY_PRESSED_D            ((RC.key.all & KEY_MASK_D) != 0)
#define KEY_PRESSED_Q            ((RC.key.all & KEY_MASK_Q) != 0)
#define KEY_PRESSED_E            ((RC.key.all & KEY_MASK_E) != 0)
#define KEY_PRESSED_SHIFT        ((RC.key.all & KEY_MASK_SHIFT) != 0)
#define KEY_PRESSED_CTRL         ((RC.key.all & KEY_MASK_CTRL) != 0)


typedef struct {
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;

    int8_t s1;
    int8_t s2;

    struct {
        int16_t x;
        int16_t y;
        int16_t z;

        uint8_t press_l;
        uint8_t press_r;
    }mouse;

    struct {
        uint16_t all;
    }key;

    //侧拨轮
    uint16_t dialWheel;

}RC_t;


extern RC_t RC;
extern uint8_t RC_Data[18];

void rcDecode(void);

#endif //DBUS_H
