#include "uav_ui.h"
#include <stdio.h>

// FreeRTOS 正确顺序
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

// 全局变量
UI_t UI;

// 这里是关键！！！ 你的工程里实例名叫 referee，不是 Referee
extern RefereeInstance_s referee;

static void UI_Graphs_Init(RefereeInstance_s *ref_instance);
static void UI_Motor_Init(RefereeInstance_s *ref_instance);
static void UI_Static_Init(RefereeInstance_s *ref_instance);
static void UI_Body_Init(RefereeInstance_s *ref_instance);

/************************ UI 注册初始化 *************************/
void UI_Register(UI_t *UI)
{
    UI->find_bool = 0;
    UI->pitch = 0;
    UI->yaw_zero = 0;
    UI->yaw_now = 0;
    UI->time = 30;

    UI->friction_left = Motor_Normal;
    UI->friction_right = Motor_Normal;
    UI->yaw_motor = Motor_Normal;
    UI->pitch_motor = Motor_Normal;
    UI->supply = Motor_Normal;
}

/************************ UI 总初始化 *************************/
void UI_Init(RefereeInstance_s *ref_instance)
{
    UI_Graphs_Init(ref_instance);
    osDelay(35);
    UI_Motor_Init(ref_instance);
    osDelay(35);
    UI_Static_Init(ref_instance);
    osDelay(35);
    UI_Body_Init(ref_instance);
    osDelay(35);
}

/************************ 图形初始化 *************************/
static void UI_Graphs_Init(RefereeInstance_s *ref_instance)
{
    // 自动瞄准框
    Referee_UI_Draw_Rectangle(ref_instance,"AM1",UI_Graph_Add,1,UI_Color_Pink,5,760,640,1160,440);
    // Yaw 角度弧
    Referee_UI_Draw_Arc(ref_instance,"YA1",UI_Graph_Add,1,UI_Color_Yellow,358,2,10,960,540,300,300);
    // Pitch 指示线
    Referee_UI_Draw_Line(ref_instance,"PI1",UI_Graph_Add,1,UI_Color_Yellow,10,1500,538,1500,542);
    // 时间条
    Referee_UI_Draw_Line(ref_instance,"TIM",UI_Graph_Add,1,UI_Color_Green,15,440,100,540,100);
    // 空图
    Referee_UI_Draw_Line(ref_instance,"NU1",UI_Graph_Invalid,1,UI_Color_Main,0,0,0,0,0);

    Referee_UI_PushUp_Graphs(ref_instance);
}

/************************ 静态图形初始化 *************************/
static void UI_Static_Init(RefereeInstance_s *ref_instance)
{
    // Yaw 刻度
    Referee_UI_Draw_Arc(ref_instance,"YA3",UI_Graph_Add,1,UI_Color_White,300,60,10,960,540,300,300);
    // Pitch 刻度
    Referee_UI_Draw_Line(ref_instance,"PI3",UI_Graph_Add,1,UI_Color_White,10,1500,490,1500,590);
    // 时间条底
    Referee_UI_Draw_Line(ref_instance,"TIM",UI_Graph_Add,1,UI_Color_Green,15,440,100,640,100);
    // 距离刻度
    Referee_UI_Draw_Line(ref_instance,"MZ2",UI_Graph_Add,3,UI_Color_Pink,3,930,500,990,500);
    Referee_UI_Draw_Line(ref_instance,"MZ3",UI_Graph_Add,3,UI_Color_Pink,3,930,520,990,520);
    Referee_UI_Draw_Line(ref_instance,"MZ4",UI_Graph_Add,3,UI_Color_Pink,3,930,540,990,540);
    Referee_UI_Draw_Line(ref_instance,"MZ5",UI_Graph_Add,3,UI_Color_Pink,3,930,560,990,560);

    Referee_UI_PushUp_Graphs(ref_instance);
}

/************************ 电机状态初始化 *************************/
static void UI_Motor_Init(RefereeInstance_s *ref_instance)
{
    // 左摩擦轮
    Referee_UI_Draw_Arc(ref_instance,"MC1",UI_Graph_Add,7,UI_Color_Green,270,90,2,1860,540,8,8);
    // 右摩擦轮
    Referee_UI_Draw_Circle(ref_instance,"MC2",UI_Graph_Add,7,UI_Color_Green,2,1860,532,8);
    // Pitch 电机
    Referee_UI_Draw_Circle(ref_instance,"PI2",UI_Graph_Add,7,UI_Color_Green,2,1780,560,10);
    // Yaw 电机
    Referee_UI_Draw_Circle(ref_instance,"YA2",UI_Graph_Add,7,UI_Color_Green,2,1800,610,10);
    // 供给
    Referee_UI_Draw_Ellipse(ref_instance,"BD2",UI_Graph_Add,7,UI_Color_Green,2,1800,660,30,10);

    Referee_UI_PushUp_Graphs(ref_instance);
}

/************************ 机身图形初始化 *************************/
static void UI_Body_Init(RefereeInstance_s *ref_instance)
{
    // 枪管
    Referee_UI_Draw_Rectangle(ref_instance,"GN1",UI_Graph_Add,9,UI_Color_Cyan,2,1840,532,1900,540);
    // 枪口
    Referee_UI_Draw_Rectangle(ref_instance,"MU1",UI_Graph_Add,9,UI_Color_Cyan,2,1880,540,1900,550);
    // 云台
    Referee_UI_Draw_Rectangle(ref_instance,"YT2",UI_Graph_Add,9,UI_Color_Cyan,2,1760,500,1840,580);
    // 颈
    Referee_UI_Draw_Rectangle(ref_instance,"NK1",UI_Graph_Add,9,UI_Color_Cyan,2,1795,580,1805,610);
    // 身体
    Referee_UI_Draw_Rectangle(ref_instance,"BDY",UI_Graph_Add,9,UI_Color_Cyan,2,1750,610,1850,710);

    Referee_UI_PushUp_Graphs(ref_instance);
}

/************************ 图形刷新 *************************/
void UI_Graphs_Refresh(RefereeInstance_s *ref_instance, UI_t UI)
{
    // 瞄准框颜色
    if (UI.find_bool == 1) {
        Referee_UI_Draw_Rectangle(ref_instance,"AM1",UI_Graph_Change,1,UI_Color_Green,5,760,640,1160,440);
    } else {
        Referee_UI_Draw_Rectangle(ref_instance,"AM1",UI_Graph_Change,1,UI_Color_Pink,5,760,640,1160,440);
    }

    // Yaw 角度
    uint8_t yaw = UI.yaw_now - UI.yaw_zero;
    Referee_UI_Draw_Arc(ref_instance,"YA1",UI_Graph_Change,1,UI_Color_Yellow,yaw-2,yaw+2,10,960,540,300,300);

    // Pitch 角度
    uint32_t pitch = 540 + UI.pitch;
    Referee_UI_Draw_Line(ref_instance,"PI1",UI_Graph_Change,1,UI_Color_Yellow,10,1500,pitch-2,1500,pitch+2);

    // 时间条
    Referee_UI_Draw_Line(ref_instance,"TIM",UI_Graph_Change,1,UI_Color_Green,15,440,100,640 - UI.time*2, 100);

    Referee_UI_PushUp_Graphs(ref_instance);
}

/************************ 电机状态刷新 *************************/
void UI_Motor_Refresh(RefereeInstance_s *ref_instance, UI_t UI)
{
    Referee_UI_Draw_Arc(ref_instance,"MC1",UI_Graph_Change,7,UI.friction_left,270,90,2,1860,540,8,8);
    Referee_UI_Draw_Circle(ref_instance,"MC2",UI_Graph_Change,7,UI.friction_right,2,1860,532,8);
    Referee_UI_Draw_Circle(ref_instance,"PI2",UI_Graph_Change,7,UI.pitch_motor,2,1780,560,10);
    Referee_UI_Draw_Circle(ref_instance,"YA2",UI_Graph_Change,7,UI.yaw_motor,2,1800,610,10);
    Referee_UI_Draw_Ellipse(ref_instance,"BD2",UI_Graph_Change,7,UI.supply,2,1800,660,30,10);

    Referee_UI_PushUp_Graphs(ref_instance);
}

// UI任务
void ui_task(void const * argument)
{
    // 等待裁判系统ID有效
    while (!Referee_Get_Robot_ID(&referee) && !Referee_Get_Client_ID(&referee)){
        osDelay(100);
    }

    UI_Register(&UI);

    // 多次初始化确保UI绘制成功
    for (int i=0; i<5; i++) {
        UI_Init(&referee);
        osDelay(35);
    }

    for (;;){
        UI.find_bool = 0;
        UI.time = 30;
        UI.pitch = 0;
        UI.yaw_now = 0;
        UI_Graphs_Refresh(&referee, UI);
        osDelay(35);

        UI.supply = Motor_Normal;
        UI.pitch_motor = Motor_Normal;
        UI.yaw_motor = Motor_Normal;
        UI.friction_left = Motor_Normal;
        UI.friction_right = Motor_Normal;
        UI_Motor_Refresh(&referee, UI);
        osDelay(35);
    }
}