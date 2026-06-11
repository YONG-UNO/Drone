//
// Created by DingYong on 2026/5/14.
//

#ifndef DRONE_CONTROL_REFRESH_H
#define DRONE_CONTROL_REFRESH_H
#include "video_transfer.h"

extern vt_t vt_data;
extern uint8_t control_mode;
extern uint8_t friction_mode;
extern uint8_t gimbal_mode;


void control_switch(void const * argument);

#endif //DRONE_CONTROL_REFRESH_H
