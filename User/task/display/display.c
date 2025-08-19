//
// Created by DingYong on 25-8-19.
//

#include "display.h"

#include "cmsis_os.h"
#include "User/connectivity/iic/bsp_iic.h"

void Display(void const * argument) {
    (void)argument;
    for (;;) {
        OLED_init();
        osDelay(100);
        OLED_operate_gram(PEN_WRITE);
        OLED_refresh_gram();

        osDelay(100);
    }
}
