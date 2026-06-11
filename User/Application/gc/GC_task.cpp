//
// Created by DingYong on 2026/5/4.
//

#include "GC_task.hpp"

#include "GC.hpp"
#include "cmsis_os.h"
#include "gimbal_control_pitch.hpp"

GravityCompensator gc;

float output_angle;
float input_torque;

void gc_task(void const * argument) {

    for (;;) {
        output_angle = gc.run(&speed_pid_CAN_RS05);
        osDelay(2000);
    }
}