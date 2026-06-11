#ifndef __UAV_UI_H__
#define __UAV_UI_H__

// 正确顺序：先FreeRTOS核心，再cmsis_os
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "dev_referee_ui.h"

typedef enum {
    Motor_Error = 5,
    Motor_Waring = 1,
    Motor_Normal = 2,
} UI_Motor_e;

typedef struct {
    uint8_t find_bool;
    uint8_t time;
    uint16_t yaw_zero;
    uint16_t yaw_now;
    uint16_t pitch;

    UI_Motor_e supply;
    UI_Motor_e yaw_motor;
    UI_Motor_e pitch_motor;
    UI_Motor_e friction_left;
    UI_Motor_e friction_right;
} UI_t;

// 外部声明
extern UI_t UI;
extern void UI_TASK(void const *argument);

void UI_Register(UI_t *UI);
void UI_Init(RefereeInstance_s *ref_instance);
void UI_Graphs_Refresh(RefereeInstance_s *ref_instance, UI_t UI);
void UI_Motor_Refresh(RefereeInstance_s *ref_instance, UI_t UI);

#endif