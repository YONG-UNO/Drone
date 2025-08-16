//
// Created by DingYong on 25-8-16.
//

#include "autoAiming.h"

#include "cmsis_os.h"
#include "usbd_cdc_if.h"

void autoAiming(void const * argument) {
    for (;;) {
        uint8_t buffer[10] = {"A\n"};
        CDC_Transmit_FS(buffer, 10);
        osDelay(1);
        USBD_Interface_fops_FS.Receive()
    }
}
