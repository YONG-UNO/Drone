/**
 * @file dev_referee_ui.h
 * @author Ma HuaCheng
 * @brief 裁判系统通信UI绘制协议协定
 * @version 0.2
 * @details 提供裁判系统基本的UI命令与协议界定(该代码基于RoboMaster裁判系统串口协议V1.3.0 2026.3 进行开发)
 * @date 2025-10-10
 * @update 2026-3-28
 * @copyright  Copyright (c) 2026 HDU—PHOENIX
 * @todo
 */

#ifndef DEV_REFEREE_UI_H
#define DEV_REFEREE_UI_H

#ifndef _packed
#define _packed __attribute__((packed))
#endif

#include "dev_referee.h"

//结构体重定向一下方便识别，原结构体名与官方串口协议结构体名字一致
typedef interaction_figure_t  referee_ui_graphic_data_t;
typedef interaction_figure_2_t referee_ui_graphic_data_2_t;
typedef interaction_figure_3_t referee_ui_graphic_data_5_t;
typedef interaction_figure_4_t referee_ui_graphic_data_7_t;
typedef ext_client_custom_character_t referee_ui_string_data_t;
typedef interaction_layer_delete_t referee_ui_delete_data_t;

typedef struct _packed   //0x0301 机器人间通信子命令头结构体
{
    uint16_t subcmd_id;
    uint16_t sender_ID;
    uint16_t receiver_ID;
} subcmd_frame_header_t;

typedef struct _packed    //一个完整的数据包(发送一个绘制图形)
{
    frame_header_t header;
    uint16_t cmd_id;
    subcmd_frame_header_t subcmd_header;
    referee_ui_graphic_data_t graphs[1];
    uint16_t crc16;
} referee_ui_graph_t;


typedef struct _packed  //一个完整的数据包(发送两个绘制图形)
{
    frame_header_t header;
    uint16_t cmd_id;
    subcmd_frame_header_t subcmd_header;
    referee_ui_graphic_data_t graphs[2];
    uint16_t crc16;
} referee_ui_graph_2_t;

typedef struct _packed  //一个完整的数据包(发送五个绘制图形)
{
    frame_header_t header;
    uint16_t cmd_id;
    subcmd_frame_header_t subcmd_header;
    referee_ui_graphic_data_t graphs[5];
    uint16_t crc16;
} referee_ui_graph_5_t;

typedef struct _packed  //一个完整的数据包(发送五个绘制图形)
{
    frame_header_t header;
    uint16_t cmd_id;
    subcmd_frame_header_t subcmd_header;
    referee_ui_graphic_data_t graphs[7];
    uint16_t crc16;
} referee_ui_graph_7_t;

typedef struct _packed  //一个完整的数据包(发送一个绘制字符)
{
    frame_header_t header;
    uint16_t cmd_id;
    subcmd_frame_header_t subcmd_header;
    referee_ui_string_data_t string;
    uint16_t crc16;
} referee_ui_string_t;

typedef struct _packed  //一个完整数据包(删除一个图层)
{
    frame_header_t header;
    uint16_t cmd_id;
    subcmd_frame_header_t subcmd_header;
    referee_ui_delete_data_t delete;
    uint16_t crc16;
} referee_ui_delete_t;


/* UI图形操作 */
typedef enum {
    UI_Graph_Invalid = 0,//空操作
    UI_Graph_Add = 1,//增加图形
    UI_Graph_Change = 2,//修改图形
    UI_Graph_Delete = 3,//删除图形
} referee_ui_graph_operate_type_e;

/* UI删除操作 */
typedef enum {
    UI_Delete_Invalid = 0,//空操作
    UI_Delete_Layer = 1,//删除图层
    UI_Delete_All = 2,//删除所有
} referee_ui_delete_type_e;

/* UI图形类型 */
typedef enum {
    UI_Graph_Line = 0, //直线
    UI_Graph_Rectangle = 1, //矩形
    UI_Graph_Circle = 2, //整圆
    UI_Graph_Ellipse = 3, //椭圆
    UI_Graph_Arc = 4, //圆弧
    UI_Graph_Float = 5, //浮点型
    UI_Graph_Int = 6, //整形
    UI_Graph_String = 7, //字符型
} referee_ui_graph_type_e;

/* UI图形颜色 */
typedef enum {
    UI_Color_Main = 0,
    UI_Color_Yellow = 1, //黄色
    UI_Color_Green = 2, //绿色
    UI_Color_Orange = 3, //橙色
    UI_Color_Purple = 4, //紫色
    UI_Color_Pink = 5, //粉色
    UI_Color_Cyan = 6, //青色
    UI_Color_Black = 7, //黑色
    UI_Color_White = 8, //白色
} referee_ui_color_type_e;


//UI辅助绘画函数(基础原始版)
/**
 * @brief 在UI上绘制直线(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI 图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param Width 线宽
 * @param StartX 起始坐标X
 * @param StartY 起始坐标Y
 * @param EndX 截止坐标X
 * @param EndY 截止坐标Y
 */
void Referee_UI_Draw_Line(RefereeInstance_s *ref_instance,
                          char GraphName[3],
                          uint8_t GraphOperate,
                          uint8_t Layer, //UI图形图层 [0,9]
                          uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                          uint16_t Width, //线宽
                          uint16_t StartX, //起始坐标X
                          uint16_t StartY, //起始坐标Y
                          uint16_t EndX, //截止坐标X
                          uint16_t EndY); //截止坐标Y


/**
 * @brief 在UI上绘制矩形(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance  裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI 图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param Width 线宽
 * @param StartX  起始坐标X
 * @param StartY 起始坐标Y
 * @param EndX 对角坐标X
 * @param EndY 对角坐标Y
 */
void Referee_UI_Draw_Rectangle(RefereeInstance_s *ref_instance,
                               char GraphName[3], //图形名 作为客户端的索引
                               uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                               uint8_t Layer, //UI图形图层 [0,9]
                               uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                               uint16_t Width, //线宽
                               uint16_t StartX, //起始坐标X
                               uint16_t StartY, //起始坐标Y
                               uint16_t EndX, //截止坐标X
                               uint16_t EndY); //截止坐标Y


/**
 * @brief 在UI上绘制圆形(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI 图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param Width 线宽
 * @param CenterX 圆心坐标X
 * @param CenterY 圆心坐标Y
 * @param Radius 半径
 */
void Referee_UI_Draw_Circle(RefereeInstance_s *ref_instance,
                            char GraphName[3], //图形名 作为客户端的索引
                            uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                            uint8_t Layer, //UI图形图层 [0,9]
                            uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                            uint16_t Width, //线宽
                            uint16_t CenterX, //圆心坐标X
                            uint16_t CenterY, //圆心坐标Y
                            uint16_t Radius); //半径

/**
 * @brief 在UI上绘制椭圆(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate  UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI 图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param Width 线宽
 * @param CenterX 圆心坐标X
 * @param CenterY 圆心坐标Y
 * @param XHalfAxis X半轴长度
 * @param YHalfAxis Y半轴长度
 */
void Referee_UI_Draw_Ellipse(RefereeInstance_s *ref_instance,
                             char GraphName[3], //图形名 作为客户端的索引
                             uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                             uint8_t Layer, //UI图形图层 [0,9]
                             uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                             uint16_t Width, //线宽
                             uint16_t CenterX, //圆心坐标X
                             uint16_t CenterY, //圆心坐标Y
                             uint16_t XHalfAxis, //X半轴长
                             uint16_t YHalfAxis); //Y半轴长


/**
 * @brief 在UI上绘制圆弧(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI 图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param StartAngle //起始角度 [0,360]
 * @param EndAngle //截止角度 [0,360]
 * @param Width //线宽
 * @param CenterX //圆心坐标X
 * @param CenterY //圆心坐标Y
 * @param XHalfAxis //X半轴长
 * @param YHalfAxis //Y半轴长
 */
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
                         uint16_t YHalfAxis); //Y半轴长

/**
 * @brief 在UI上绘制浮点数(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param NumberSize 字体大小
 * @param Width 线宽
 * @param StartX 起始坐标X
 * @param StartY 起始坐标Y
 * @param FloatData 数字内容
 */
void Referee_UI_Draw_Float(RefereeInstance_s *ref_instance,
                           char GraphName[3], //图形名 作为客户端的索引
                           uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                           uint8_t Layer, //UI图形图层 [0,9]
                           uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                           uint16_t NumberSize, //字体大小
                           uint16_t Width, //线宽
                           uint16_t StartX, //起始坐标X
                           uint16_t StartY, //起始坐标Y
                           float FloatData); //数字内容

/**
 * @brief 在UI上绘制整数(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param GraphName 图形名 作为客户端的索引
 * @param GraphOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI 图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param NumberSize 字体大小
 * @param Width 线宽
 * @param StartX 起始坐标X
 * @param StartY 起始坐标Y
 * @param IntData 数字内容
 */
void Referee_UI_Draw_Int(RefereeInstance_s *ref_instance,
                         char GraphName[3], //图形名 作为客户端的索引
                         uint8_t GraphOperate, //UI图形操作 对应UI_Graph_XXX的4种操作
                         uint8_t Layer, //UI图形图层 [0,9]
                         uint8_t Color, //UI图形颜色 对应UI_Color_XXX的9种颜色
                         uint16_t NumberSize, //字体大小
                         uint16_t Width, //线宽
                         uint16_t StartX, //起始坐标X
                         uint16_t StartY, //起始坐标Y
                         int32_t IntData); //数字内容

/**
 * @brief 在UI上绘制字符串(数据存入缓存区，出发推送函数后发送数据)
 * @param ref_instance 裁判系统实例
 * @param StringName 图形名 作为客户端的索引
 * @param StringOperate UI图形操作 对应UI_Graph_XXX的4种操作
 * @param Layer UI图形图层 [0,9]
 * @param Color UI图形颜色 对应UI_Color_XXX的9种颜色
 * @param CharSize 字体大小
 * @param StringLength 字符串长度
 * @param Width 线宽
 * @param StartX 起始坐标X
 * @param StartY 起始坐标Y
 * @param StringData 字符串内容
 */
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
                            char *StringData); //字符串内容

/**
 * @brief 推送所有本地暂存所有数据(调用推送图形数据，字符串数据，删除图层数据,所以仍需保证图形缓存为1,2,5,7个，其他数据长度无法发送)
 * @param ref_instance 裁判系统实例
 */
void Referee_UI_PushUp_All(RefereeInstance_s *ref_instance);

/**
 * @brief 推送所有本地暂存的图形数据
 * @param ref_instance 裁判系统实例
 */
void Referee_UI_PushUp_Graphs(RefereeInstance_s *ref_instance);

/**
 * @brief 推送所有本地暂存的字符串数据
 * @param ref_instance 裁判系统实例
 */
void Referee_UI_PushUp_String(RefereeInstance_s *ref_instance);

/**
 * @brief 推送所有本地暂存的删除图层数据
 * @param ref_instance 裁判系统实例
 */
void Referee_UI_PushUp_Delete(RefereeInstance_s *ref_instance);


#endif
