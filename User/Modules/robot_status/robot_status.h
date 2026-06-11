//
// Created by DingYong on 2026/5/18.
//

#ifndef DRONE_ROBOT_STATUS_H
#define DRONE_ROBOT_STATUS_H
#include <stdint.h>

#define BUF_SIZE 128

typedef struct __attribute__((packed)) {
    uint16_t heat_now;
    uint16_t heat_max;
    uint16_t cool_rate;
} robot_status_t;

extern robot_status_t robot_status;

extern uint8_t dma_buf[BUF_SIZE];
extern uint8_t rec_buf[BUF_SIZE];
extern uint8_t rec_len;

extern uint16_t heat_now;
extern uint16_t heat_max;
extern uint16_t cool_rate;

void robot_status_init(void);
void robot_status_process(void);

#endif //DRONE_ROBOT_STATUS_H
