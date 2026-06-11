/**
 * @file dev_referee_ui.c
 * @author Ma HuaCheng
 * @brief 裁判系统通信UI绘制协议协定
 * @version 0.2
 * @details 提供裁判系统基本的UI命令与协议界定(该代码基于RoboMaster裁判系统串口协议V1.3.0 2026.3 进行开发)
 * @date 2025-10-10
 * @update 2026-3-28
 * @copyright  Copyright (c) 2026 HDU—PHOENIX
 * @todo
 */


#include "dev_referee_ui.h"

#include <string.h>

#include "alg_crc.h"
//发送数据存储缓存区
referee_ui_graphic_data_t UI_Graph_data[7];
referee_ui_string_data_t UI_String_data[1];
referee_ui_delete_data_t UI_Delete_data[1];
//计数区(用来统计当前缓存区的数据存储数量)
uint16_t referee_ui_graphic_cnt = 0;
uint16_t referee_ui_string_cnt = 0;
uint16_t referee_ui_delete_cnt = 0;
//发送完整结构体存储区
referee_ui_graph_t UI_Graph1;
referee_ui_graph_2_t UI_Graph2;
referee_ui_graph_5_t UI_Graph5;
referee_ui_graph_7_t UI_Graph7;
referee_ui_delete_t UI_Delete;
referee_ui_string_t UI_String;

void Referee_UI_Draw_Line(RefereeInstance_s *ref_instance,
                          char GraphName[3],
                          uint8_t GraphOperate,
                          uint8_t Layer, //UI图形图层 [0,9]
                          uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                          uint16_t Width, //线宽
                          uint16_t StartX, //起始坐标X
                          uint16_t StartY, //起始坐标Y
                          uint16_t EndX, //截止坐标X
                          uint16_t EndY) //截止坐标Y
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Line;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x = StartX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = StartY;
    UI_Graph_data[referee_ui_graphic_cnt].details_d = EndX;
    UI_Graph_data[referee_ui_graphic_cnt].details_e = EndY;
    referee_ui_graphic_cnt ++;
}


void Referee_UI_Draw_Rectangle(RefereeInstance_s *ref_instance,
                               char GraphName[3], //图形名 作为客户端的索引
                               uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                               uint8_t Layer, //UI图形图层 [0,9]
                               uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                               uint16_t Width, //线宽
                               uint16_t StartX, //起始坐标X
                               uint16_t StartY, //起始坐标Y
                               uint16_t EndX, //截止坐标X
                               uint16_t EndY)//截止坐标Y
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Rectangle;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x = StartX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = StartY;
    UI_Graph_data[referee_ui_graphic_cnt].details_d = EndX;
    UI_Graph_data[referee_ui_graphic_cnt].details_e = EndY;
    referee_ui_graphic_cnt ++;
}



void Referee_UI_Draw_Circle(RefereeInstance_s *ref_instance,
                            char GraphName[3], //图形名 作为客户端的索引
                            uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                            uint8_t Layer, //UI图形图层 [0,9]
                            uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                            uint16_t Width, //线宽
                            uint16_t CenterX, //圆心坐标X
                            uint16_t CenterY, //圆心坐标Y
                            uint16_t Radius) //半径
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Circle;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x = CenterX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = CenterY;
    UI_Graph_data[referee_ui_graphic_cnt].details_c = Radius;
    referee_ui_graphic_cnt ++;
}


void Referee_UI_Draw_Ellipse(RefereeInstance_s *ref_instance,
                             char GraphName[3], //图形名 作为客户端的索引
                             uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                             uint8_t Layer, //UI图形图层 [0,9]
                             uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                             uint16_t Width, //线宽
                             uint16_t CenterX, //圆心坐标X
                             uint16_t CenterY, //圆心坐标Y
                             uint16_t XHalfAxis, //X半轴长
                             uint16_t YHalfAxis) //Y半轴长
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Ellipse;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x =CenterX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = CenterY;
    UI_Graph_data[referee_ui_graphic_cnt].details_d = XHalfAxis;
    UI_Graph_data[referee_ui_graphic_cnt].details_e = YHalfAxis;
    referee_ui_graphic_cnt ++;
}

void Referee_UI_Draw_Arc(RefereeInstance_s *ref_instance,
                         char GraphName[3], //图形名 作为客户端的索引
                         uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                         uint8_t Layer, //UI图形图层 [0,9]
                         uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                         uint16_t StartAngle, //起始角度 [0,360]
                         uint16_t EndAngle, //截止角度 [0,360]
                         uint16_t Width, //线宽
                         uint16_t CenterX, //圆心坐标X
                         uint16_t CenterY, //圆心坐标Y
                         uint16_t XHalfAxis, //X半轴长
                         uint16_t YHalfAxis)//Y半轴长
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Arc;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].details_a = StartAngle;
    UI_Graph_data[referee_ui_graphic_cnt].details_b = EndAngle;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x =CenterX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = CenterY;
    UI_Graph_data[referee_ui_graphic_cnt].details_d = XHalfAxis;
    UI_Graph_data[referee_ui_graphic_cnt].details_e = YHalfAxis;
    referee_ui_graphic_cnt ++;
}

void Referee_UI_Draw_Float(RefereeInstance_s *ref_instance,
                           char GraphName[3], //图形名 作为客户端的索引
                           uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                           uint8_t Layer, //UI图形图层 [0,9]
                           uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                           uint16_t NumberSize, //字体大小
                           uint16_t Width, //线宽
                           uint16_t StartX, //起始坐标X
                           uint16_t StartY, //起始坐标Y
                           float FloatData) //数字内容
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Float;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].details_a = NumberSize;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x =StartX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = StartY;
    int32_t IntData = FloatData * 1000;
    UI_Graph_data[referee_ui_graphic_cnt].details_c = (IntData & 0x000003ff) >>  0;
    UI_Graph_data[referee_ui_graphic_cnt].details_d = (IntData & 0x001ffc00) >> 10;
    UI_Graph_data[referee_ui_graphic_cnt].details_e = (IntData & 0xffe00000) >> 21;
    referee_ui_graphic_cnt ++;
}


void Referee_UI_Draw_Int(RefereeInstance_s *ref_instance,
                         char GraphName[3], //图形名 作为客户端的索引
                         uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                         uint8_t Layer, //UI图形图层 [0,9]
                         uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                         uint16_t NumberSize, //字体大小
                         uint16_t Width, //线宽
                         uint16_t StartX, //起始坐标X
                         uint16_t StartY, //起始坐标Y
                         int32_t IntData)//数字内容
{
    if (referee_ui_graphic_cnt >= 7) {
        return;
    }
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[0] = GraphName[0];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[1] = GraphName[1];
    UI_Graph_data[referee_ui_graphic_cnt].figure_name[2] = GraphName[2];
    UI_Graph_data[referee_ui_graphic_cnt].operate_type = GraphOperate;
    UI_Graph_data[referee_ui_graphic_cnt].figure_type = UI_Graph_Int;
    UI_Graph_data[referee_ui_graphic_cnt].layer = Layer;
    UI_Graph_data[referee_ui_graphic_cnt].color = Color;
    UI_Graph_data[referee_ui_graphic_cnt].details_a = NumberSize;
    UI_Graph_data[referee_ui_graphic_cnt].width = Width;
    UI_Graph_data[referee_ui_graphic_cnt].start_x =StartX;
    UI_Graph_data[referee_ui_graphic_cnt].start_y = StartY;
    UI_Graph_data[referee_ui_graphic_cnt].details_c = (IntData & 0x000003ff) >>  0;
    UI_Graph_data[referee_ui_graphic_cnt].details_d = (IntData & 0x001ffc00) >> 10;
    UI_Graph_data[referee_ui_graphic_cnt].details_e = (IntData & 0xffe00000) >> 21;
    referee_ui_graphic_cnt ++;
}


void Referee_UI_Draw_String(RefereeInstance_s *ref_instance,
                            char StringName[3], //图形名 作为客户端的索引
                            uint8_t StringOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                            uint8_t Layer, //UI图形图层 [0,9]
                            uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                            uint16_t CharSize, //字体大小
                            uint16_t StringLength, //字符串长度
                            uint16_t Width, //线宽
                            uint16_t StartX, //起始坐标X
                            uint16_t StartY, //起始坐标Y
                            char *StringData)//字符串内容
{
    if (referee_ui_string_cnt != 0 ) {
        return;
    }
    UI_String_data[referee_ui_string_cnt].string_name[0] = StringName[0];
    UI_String_data[referee_ui_string_cnt].string_name[1] = StringName[1];
    UI_String_data[referee_ui_string_cnt].string_name[2] = StringName[2];
    UI_String_data[referee_ui_string_cnt].operate_type = StringOperate;
    UI_String_data[referee_ui_string_cnt].graphic_type = UI_Graph_String;
    UI_String_data[referee_ui_string_cnt].layer = Layer;
    UI_String_data[referee_ui_string_cnt].color = Color;
    UI_String_data[referee_ui_string_cnt].details_a = CharSize;
    UI_String_data[referee_ui_string_cnt].details_b = StringLength;
    UI_String_data[referee_ui_string_cnt].width = Width;
    UI_String_data[referee_ui_string_cnt].start_x =StartX;
    UI_String_data[referee_ui_string_cnt].start_y = StartY;
    for(int i = 0; i < StringLength; i ++) {
        UI_String_data[referee_ui_string_cnt].stringdata[i] = *StringData ++;
    }
    referee_ui_string_cnt++;
}


void Referee_UI_PushUp_All(RefereeInstance_s *ref_instance) {
    Referee_UI_PushUp_Graphs(ref_instance);
    Referee_UI_PushUp_String(ref_instance);
    Referee_UI_PushUp_Delete(ref_instance);

}



void Referee_UI_PushUp_Graphs(RefereeInstance_s *ref_instance) {
    switch (referee_ui_graphic_cnt) {
        uint8_t* data;
        uint16_t struct_size;
        case 1:
            UI_Graph1.header.SOF = REF_HEADER_SOF;
            UI_Graph1.header.Data_Length = 6 + 1*15;
            UI_Graph1.header.Seq ++;
            UI_Graph1.header.CRC8 = CRC08_Calculate(((uint8_t *)&UI_Graph1.header), 4);
            UI_Graph1.cmd_id = STUDENT_INTERACTIVE_DATA_CMD_ID;
            UI_Graph1.subcmd_header.subcmd_id = UI_CREATE_ONE_SUBCMD_ID;
            UI_Graph1.subcmd_header.sender_ID = Referee_Get_Robot_ID(ref_instance);
            UI_Graph1.subcmd_header.receiver_ID = UI_Graph1.subcmd_header.sender_ID +256;
            memcpy(&UI_Graph1.graphs[0],&UI_Graph_data[0],sizeof(referee_ui_graphic_data_t));
            UI_Graph1.crc16 = CRC16_Calculate(((uint8_t *)&UI_Graph1),sizeof(referee_ui_graph_t)-2);
            data = (uint8_t *)&UI_Graph1;

            //清空缓存
            referee_ui_graphic_cnt = 0;
            memset(&UI_Graph_data,0,sizeof(UI_Graph_data));

            if (!Uart_Transmit_Len(ref_instance->uart_instance,data,sizeof(referee_ui_graph_t))) {
            }
            break;
        case 2:
            UI_Graph2.header.SOF = REF_HEADER_SOF;
            UI_Graph2.header.Data_Length = 6 + 2*15;
            UI_Graph2.header.Seq ++;
            UI_Graph2.header.CRC8 = CRC08_Calculate(((uint8_t *)&UI_Graph2.header), 4);

            UI_Graph2.cmd_id = STUDENT_INTERACTIVE_DATA_CMD_ID;
            UI_Graph2.subcmd_header.subcmd_id = UI_CREATE_TWO_SUBCMD_ID;
            UI_Graph2.subcmd_header.sender_ID = Referee_Get_Robot_ID(ref_instance);
            UI_Graph2.subcmd_header.receiver_ID = UI_Graph2.subcmd_header.sender_ID +256;
            memcpy(&UI_Graph2.graphs[0],&UI_Graph_data[0],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph2.graphs[1],&UI_Graph_data[1],sizeof(referee_ui_graphic_data_t));
            UI_Graph2.crc16 = CRC16_Calculate(((uint8_t *)&UI_Graph2),sizeof(referee_ui_graph_2_t)-2);
            data = (uint8_t *)&UI_Graph2;

            //清空缓存
            referee_ui_graphic_cnt = 0;
            memset(&UI_Graph_data,0,sizeof(UI_Graph_data));

            if (!Uart_Transmit_Len(ref_instance->uart_instance,data,sizeof(referee_ui_graph_2_t))) {

            }
            break;
        case 5:
            UI_Graph5.header.SOF = REF_HEADER_SOF;
            UI_Graph5.header.Data_Length = 6 + 5*15;
            UI_Graph5.header.Seq ++;
            UI_Graph5.header.CRC8 = CRC08_Calculate(((uint8_t *)&UI_Graph5.header), 4);

            UI_Graph5.cmd_id = STUDENT_INTERACTIVE_DATA_CMD_ID;
            UI_Graph5.subcmd_header.subcmd_id = UI_CREATE_FIVE_SUBCMD_ID;
            UI_Graph5.subcmd_header.sender_ID = Referee_Get_Robot_ID(ref_instance);
            UI_Graph5.subcmd_header.receiver_ID = UI_Graph5.subcmd_header.sender_ID +256;
            memcpy(&UI_Graph5.graphs[0],&UI_Graph_data[0],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph5.graphs[1],&UI_Graph_data[1],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph5.graphs[2],&UI_Graph_data[2],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph5.graphs[3],&UI_Graph_data[3],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph5.graphs[4],&UI_Graph_data[4],sizeof(referee_ui_graphic_data_t));

            UI_Graph5.crc16 = CRC16_Calculate(((uint8_t *)&UI_Graph5),sizeof(referee_ui_graph_5_t)-2);
            data = (uint8_t *)&UI_Graph5;

            //清空缓存
            referee_ui_graphic_cnt = 0;
            memset(&UI_Graph_data,0,sizeof(UI_Graph_data));

            if (!Uart_Transmit_Len(ref_instance->uart_instance,data,sizeof(referee_ui_graph_5_t))) {
            }
            break;
        case 7 :
            UI_Graph7.header.SOF = REF_HEADER_SOF;
            UI_Graph7.header.Data_Length = 6 + 7*15;
            UI_Graph7.header.Seq ++;
            UI_Graph7.header.CRC8 = CRC08_Calculate(((uint8_t *)&UI_Graph7.header), 4);

            UI_Graph7.cmd_id = STUDENT_INTERACTIVE_DATA_CMD_ID;
            UI_Graph7.subcmd_header.subcmd_id = UI_CREATE_SEVEN_SUBCMD_ID;
            UI_Graph7.subcmd_header.sender_ID = Referee_Get_Robot_ID(ref_instance);
            UI_Graph7.subcmd_header.receiver_ID = UI_Graph7.subcmd_header.sender_ID +256;
            memcpy(&UI_Graph7.graphs[0],&UI_Graph_data[0],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph7.graphs[1],&UI_Graph_data[1],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph7.graphs[2],&UI_Graph_data[2],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph7.graphs[3],&UI_Graph_data[3],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph7.graphs[4],&UI_Graph_data[4],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph7.graphs[5],&UI_Graph_data[5],sizeof(referee_ui_graphic_data_t));
            memcpy(&UI_Graph7.graphs[6],&UI_Graph_data[6],sizeof(referee_ui_graphic_data_t));

            UI_Graph7.crc16 = CRC16_Calculate(((uint8_t *)&UI_Graph7),sizeof(referee_ui_graph_7_t)-2);
            data = (uint8_t *)&UI_Graph7;

            //清空缓存
            referee_ui_graphic_cnt = 0;
            memset(&UI_Graph_data,0,sizeof(UI_Graph_data));

            if (!Uart_Transmit_Len(ref_instance->uart_instance,data,sizeof(referee_ui_graph_7_t))) {
            }
            break;
        default:
            return;
    }



}

void Referee_UI_PushUp_String(RefereeInstance_s *ref_instance) {
    UI_String.header.SOF = REF_HEADER_SOF;
    UI_String.header.Data_Length = 6 + 45;
    UI_String.header.Seq ++;
    UI_String.header.CRC8 = CRC08_Calculate(((uint8_t *)&UI_String.header), 4);
    UI_String.cmd_id = STUDENT_INTERACTIVE_DATA_CMD_ID;
    UI_String.subcmd_header.subcmd_id = UI_CREATE_CHARACTER_SUBCMD_ID;
    UI_String.subcmd_header.sender_ID = Referee_Get_Robot_ID(ref_instance);
    UI_String.subcmd_header.receiver_ID = UI_String.subcmd_header.sender_ID +256;
    memcpy(&UI_String.string,&UI_String_data[0],sizeof(referee_ui_string_data_t));
    UI_String.crc16 = CRC16_Calculate(((uint8_t *)&UI_String),sizeof(referee_ui_string_t)-2);
    uint8_t *data = (uint8_t *)&UI_String;

    //清空缓存
    memset(&UI_String_data,0,sizeof(UI_String_data));

    if (!Uart_Transmit_Len(ref_instance->uart_instance,data,sizeof(referee_ui_string_t))) {
        return;
    }
}

void Referee_UI_PushUp_Delete(RefereeInstance_s *ref_instance) {
    UI_Delete.header.SOF = REF_HEADER_SOF;
    UI_Delete.header.Data_Length = 6 + 2;
    UI_Delete.header.Seq ++;
    UI_Delete.header.CRC8 = CRC08_Calculate(((uint8_t *)&UI_Delete.header), 4);
    UI_Delete.cmd_id = STUDENT_INTERACTIVE_DATA_CMD_ID;
    UI_Delete.subcmd_header.subcmd_id = UI_LAYER_DELETE_SUBCMD_ID;
    UI_Delete.subcmd_header.sender_ID = Referee_Get_Robot_ID(ref_instance);
    UI_Delete.subcmd_header.receiver_ID = UI_Delete.subcmd_header.sender_ID +256;
    memcpy(&UI_Delete.delete,&UI_Delete_data[0],sizeof(referee_ui_delete_data_t));
    UI_Delete.crc16 = CRC16_Calculate(((uint8_t *)&UI_Delete),sizeof(referee_ui_delete_t)-2);
    uint8_t *data = (uint8_t *)&UI_Delete;

    //清空缓存
    memset(&UI_Delete_data,0,sizeof(UI_Delete_data));

    if (!Uart_Transmit_Len(ref_instance->uart_instance,data,sizeof(referee_ui_delete_t))) {
        return;
    }

}