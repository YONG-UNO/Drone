//
// Created by DingYong on 2026/5/20.
//

#include "ui_show.h"

#include "cmsis_os.h"
#include "ui.h"

void ui_task(void const * argument) {
    ui_clear();
    osDelay(100);

    for (;;) {
        ui_red_circle();
        // ui_line(50,50,300,300,UI_RED,4);
        osDelay(100);
    }
}

