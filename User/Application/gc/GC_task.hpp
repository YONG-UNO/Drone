//
// Created by DingYong on 2026/5/4.
//

#ifndef DRONE_GC_TASK_HPP
#define DRONE_GC_TASK_HPP

extern float output_angle;
extern float target_torque;

#ifdef __cplusplus
extern "C" {
#endif

    void gc_task(void const * argument);

#ifdef __cplusplus
}
#endif

#endif //DRONE_GC_TASK_HPP
