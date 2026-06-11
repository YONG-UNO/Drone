//
// Created by DingYong on 2026/5/17.
//

#include "led_task.h"

#include "WS2812b.h"
#include "cmsis_os.h"
#include "control_switch.h"

void led(void const * argument) {

    for (;;) {
        ws2812b_l_set(0,255,100);
        ws2812b_l_on();

        ws2812b_r_set(0,255,100);
        ws2812b_r_on();

        // left
        if (vt_data.ascii_key == 'c' && vt_data.key == 36) {
            ws2812b_l_set(255,0,0);
            ws2812b_r_set(0,255,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(200);

            ws2812b_l_set(0,0,0);
            ws2812b_r_set(0,255,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(200);
        }

        // right
        if (vt_data.ascii_key == 'c' && vt_data.key == 40) {
            ws2812b_l_set(0,255,0);
            ws2812b_r_set(255,0,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(200);

            ws2812b_l_set(0,255,0);
            ws2812b_r_set(0,0,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(200);
        }

        // up
        if (vt_data.ascii_key == 'c' && vt_data.key == 33) {
            ws2812b_l_set(0,255,0);
            ws2812b_r_set(0,255,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(300);

            ws2812b_l_set(0,0,0);
            ws2812b_r_set(0,0,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(300);
        }
        // down
        if (vt_data.ascii_key == 'c' && vt_data.key == 34) {
            ws2812b_l_set(255,0,0);
            ws2812b_r_set(255,0,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(300);

            ws2812b_l_set(0,0,0);
            ws2812b_r_set(0,0,0);
            ws2812b_l_on();
            ws2812b_r_on();
            osDelay(300);
        }

        osDelay(2);
    }
}
