//
// Created by DingYong on 2026/5/14.
//

#include "control_switch.h"

#include "cmsis_os.h"

uint8_t control_mode  = 0;
uint8_t s_key_pressed = 0;

uint8_t friction_mode = 0;
uint8_t E_key_pressed = 0;

uint8_t gimbal_mode   = 0;
uint8_t Q_key_pressed = 0;

uint8_t offset_yaw_1  = 0;
uint8_t a_key_pressed = 0;
uint8_t offset_yaw_0  = 0;




void control_switch(void const * argument){

    for (;;) {

        if (vt_data.ascii_key == 's' && s_key_pressed == 0) {
            // 按下shift,翻转开关
            control_mode = !control_mode;
            s_key_pressed = 1;  // 锁住
        }
        // 松开shift,解锁
        if (vt_data.ascii_key != 's') {
            s_key_pressed = 0;
        }



        if (vt_data.ascii_key == 'E' && E_key_pressed == 0) {
            // 按下shift,翻转开关
            friction_mode = !friction_mode;
            E_key_pressed = 1;  // 锁住
        }
        // 松开shift,解锁
        if (vt_data.ascii_key != 'E') {
            E_key_pressed = 0;
        }



        if (vt_data.ascii_key == 'Q' && Q_key_pressed == 0) {
            // 按下shift,翻转开关
            gimbal_mode = !gimbal_mode;
            Q_key_pressed = 1;  // 锁住
        }
        // 松开shift,解锁
        if (vt_data.ascii_key != 'Q') {
            Q_key_pressed = 0;
        }


        osDelay(100);
    }


}