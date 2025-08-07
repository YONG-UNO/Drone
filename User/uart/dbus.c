//
// Created by DingYong on 25-8-5.
//

#include "dbus.h"


void rcDecode(void) {
    RC.ch0 = (((uint16_t)RC_Data[0] | (uint16_t)RC_Data[1] << 8) & 0x07FF) - 1024;  //移位运算符优先级高于位运算符
    RC.ch1 = (((uint16_t)RC_Data[1] >> 3 | (uint16_t)RC_Data[2] << 5) & 0x07FF) - 1024;
    RC.ch2 = (((uint16_t)RC_Data[2] >> 6 | (uint16_t)RC_Data[3] << 2 | (uint16_t)RC_Data[4] << 10) & 0x07FF) - 1024;
    RC.ch3 = (((uint16_t)RC_Data[4] >> 1 | (uint16_t)RC_Data[5] << 7) & 0x07FF) - 1024;

    RC.s1 = RC_Data[5] >> 6 & 0x03;    //跳过数组[5]的第2,3,4位,s1使用7,8位,s2使用5,6位
    RC.s2 = RC_Data[5] >> 4 & 0x03;    //目的:使mouse.press_x对齐

    RC.mouse.x = (uint16_t)RC_Data[6] | (uint16_t)RC_Data[7] << 8;
    RC.mouse.y = (uint16_t)RC_Data[8] | (uint16_t)RC_Data[9] << 8;
    RC.mouse.z = (uint16_t)RC_Data[10] | (uint16_t)RC_Data[11] << 8;
    RC.mouse.press_l = RC_Data[12];
    RC.mouse.press_r = RC_Data[13];

    RC.key.all = (uint16_t)RC_Data[14] | (uint16_t)RC_Data[15];

    RC.dialWheel = ((uint16_t)RC_Data[16] | (uint16_t)RC_Data[17] << 8);
}

//补充:错误消息
//侧拨轮

/* 摇杆映射
 *            左摇杆:       |           右摇杆:
 *             ch3:        |            ch1:
 *             660         |            660
 *  ch2: -660        660   | ch4: -660        660
 *             -660        |            -660
 */