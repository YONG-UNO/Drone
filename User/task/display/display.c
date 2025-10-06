//
// Created by DingYong on 25-8-19.
//


#include "cmsis_os.h"
#include "display.h"

#include "dbus.h"
#include "User/device/ssd1306/ssd1306.h"


void Display(void const * argument) {
    (void)argument;

    // ReSharper disable once CppDFAEndlessLoop
    for (;;) {
        // OLED_draw_point(50,28,PEN_WRITE);
        // OLED_draw_point(50,29,PEN_WRITE);
        //
        // OLED_draw_point(61,21,PEN_WRITE);
        // OLED_draw_point(62,21,PEN_WRITE);
        // OLED_draw_point(63,21,PEN_WRITE);
        // OLED_draw_point(64,21,PEN_WRITE);
        // OLED_draw_point(65,21,PEN_WRITE);
        // OLED_draw_point(66,21,PEN_WRITE);
        // OLED_draw_point(67,21,PEN_WRITE);
        // OLED_draw_point(68,21,PEN_WRITE);
        //
        // OLED_draw_point(62,21,PEN_WRITE);
        // OLED_draw_point(63,22,PEN_WRITE);
        // OLED_draw_point(64,22,PEN_WRITE);
        // OLED_draw_point(65,22,PEN_WRITE);
        // OLED_draw_point(66,22,PEN_WRITE);
        // OLED_draw_point(67,22,PEN_WRITE);
        // OLED_draw_point(68,22,PEN_WRITE);
        // OLED_draw_point(69,22,PEN_WRITE);
        //
        // OLED_draw_point(61,23,PEN_WRITE);
        // OLED_draw_point(62,23,PEN_WRITE);
        // OLED_draw_point(63,23,PEN_WRITE);
        //
        // OLED_draw_point(68,23,PEN_WRITE);
        // OLED_draw_point(69,23,PEN_WRITE);
        // OLED_draw_point(70,23,PEN_WRITE);
        //
        // OLED_draw_point(62,24,PEN_WRITE);
        // OLED_draw_point(69,24,PEN_WRITE);
        // OLED_draw_point(70,24,PEN_WRITE);
        // OLED_draw_point(71,24,PEN_WRITE);
        //
        // OLED_draw_point(61,25,PEN_WRITE);
        // OLED_draw_point(64,25,PEN_WRITE);
        // OLED_draw_point(70,25,PEN_WRITE);
        // OLED_draw_point(71,25,PEN_WRITE);
        //
        // OLED_draw_point(61,26,PEN_WRITE);
        // OLED_draw_point(70,26,PEN_WRITE);
        // OLED_draw_point(71,26,PEN_WRITE);
        //
        // OLED_draw_point(62,27,PEN_WRITE);
        // OLED_draw_point(70,27,PEN_WRITE);
        //
        // OLED_draw_point(62,28,PEN_WRITE);
        // OLED_draw_point(63,28,PEN_WRITE);
        // OLED_draw_point(69,28,PEN_WRITE);
        //
        // OLED_draw_point(63,29,PEN_WRITE);
        // OLED_draw_point(68,29,PEN_WRITE);
        // OLED_draw_point(69,29,PEN_WRITE);
        //
        // OLED_draw_point(64,30,PEN_WRITE);
        // OLED_draw_point(65,30,PEN_WRITE);
        // OLED_draw_point(66,30,PEN_WRITE);
        // OLED_draw_point(67,30,PEN_WRITE);
        //
        // OLED_draw_point(65,31,PEN_WRITE);
        // OLED_draw_point(66,31,PEN_WRITE);
        // OLED_draw_point(67,31,PEN_WRITE);
        //
        // for (int j = 32;j <= 42;j++) {
        //     for (int i = 66; i <= 67;i++) {
        //         OLED_draw_point(i,j,PEN_WRITE);
        //     }
        // }
        //
        // for (int j = 32;j <= 37;j++) {
        //     OLED_draw_point(69,j,PEN_WRITE);
        // }
        //
        // OLED_draw_point(65,42,PEN_WRITE);
        //
        // OLED_draw_point(64,33,PEN_WRITE);
        // OLED_draw_point(63,34,PEN_WRITE);
        // OLED_draw_point(62,35,PEN_WRITE);
        // OLED_draw_point(61,35,PEN_WRITE);


        OLED_draw_data(0xA0,0,RC.ch0,RC.ch1);
        OLED_refresh_gram();

        osDelay(10);
    }
}
