/**
 * @file dev_referee_protocol.h
 * @author Ma HuaCheng
 * @brief 裁判系统通信模块协议界定
 * @version 0.2
 * @details 提供裁判系统基本的命令与协议界定(该代码基于RoboMaster裁判系统串口协议V1.3.0 2026.3 进行开发)
 * @date 2025-10-10
 * @update 2026-3-28
 * @copyright  Copyright (c) 2026 HDU—PHOENIX
 * @note 部分信息可能有误，所有信息以官方文档为主
 */
#ifndef DEV_REFEREE_PROTOCOL_H
#define DEV_REFEREE_PROTOCOL_H

#ifndef _packed
#define _packed __attribute__((packed))
#endif

#include "stdint.h"
typedef enum {
    Robot_ID_Red_Hero         = 1,  //红方英雄
    Robot_ID_Red_Engineer     = 2,  //红方工程
    Robot_ID_Red_Infantry3    = 3,  //红方步兵3
    Robot_ID_Red_Infantry4    = 4,  //红方步兵4
    Robot_ID_Red_Infantry5    = 5,  //红方步兵5
    Robot_ID_Red_Aerial       = 6,  //红方无人机
    Robot_ID_Red_Sentry       = 7,  //红方哨兵
    Robot_ID_Red_Darts        = 8,  //红方飞镖
    Robot_ID_Red_Radar        = 9,  //红方雷达
    Robot_ID_Red_Outpost      = 10, //红方前哨站
    Robot_ID_Red_Base         = 11, //红方基地

    Robot_ID_Blue_Hero        = 101, //蓝方英雄
    Robot_ID_Blue_Engineer    = 102, //蓝方工程
    Robot_ID_Blue_Infantry3   = 103, //蓝方步兵3
    Robot_ID_Blue_Infantry4   = 104, //蓝方步兵4
    Robot_ID_Blue_Infantry5   = 105, //蓝方步兵5
    Robot_ID_Blue_Aerial      = 106, //蓝方无人机
    Robot_ID_Blue_Sentry      = 107, //蓝方哨兵
    Robot_ID_Blue_Darts       = 108, //蓝方飞镖
    Robot_ID_Blue_Radar       = 109, //蓝方雷达
    Robot_ID_Blue_Outpost     = 110, //蓝方前哨站
    Robot_ID_Blue_Base        = 111  //蓝方基地
} referee_robot_id_e;


typedef enum {
    BLUE = 0,
    RED  = 1
} referee_color_e;


typedef enum
{   //各命令ID
    GAME_STATE_CMD_ID                 = 0x0001,  //比赛状态数据 常规链路 数据长度:11 1Hz 所有机器人可接收
    GAME_RESULT_CMD_ID                = 0x0002,  //比赛结果数据 常规链路 数据长度:1 仅在比赛结束后发送 所有机器人可接收
    GAME_ROBOT_HP_CMD_ID              = 0x0003,  //机器人血量数据 常规链路 数据长度:16 3Hz 所有机器人可接收

    FIELD_EVENTS_CMD_ID               = 0x0101,  //场地事件数据 常规链路 数据长度:4 1Hz 所有机器人可接收
    REFEREE_WARNING_CMD_ID            = 0x0104,  //裁判警告信息 常规链路 数据长度:3 仅在违规时发送,其余时间1Hz 所有机器人可接收
    DART_REMAINING_TIME_CMD_ID        = 0x0105,  //飞镖发射相关数据 常规链路 数据长度:3 1Hz 所有机器人可接收

    ROBOT_STATE_CMD_ID                = 0x0201,  //机器人性能体系数据 常规链路 数据长度:13 10Hz 对应机器人可接收
    POWER_HEAT_DATA_CMD_ID            = 0x0202,  //实时底盘缓冲能量和射击热量数据 常规链路 数据长度:14 10Hz 对应机器人可接收
    ROBOT_POS_CMD_ID                  = 0x0203,  //机器人位置数据 常规链路 数据长度:16 1Hz 对应机器人可接收
    BUFF_MUSK_CMD_ID                  = 0x0204,  //机器人增益与底盘能量数据 常规链路 数据长度:8 3Hz 对应机器人可接收
    ROBOT_HURT_CMD_ID                 = 0x0206,  //伤害状态数据 常规链路 数据长度:1 发生伤害后发送 对应机器人可接收
    SHOOT_DATA_CMD_ID                 = 0x0207,  //实时射击信息 常规链路 数据长度:7 弹丸发射后发送 对应机器人可接收
    BULLET_REMAINING_CMD_ID           = 0x0208,  //子弹剩余发射数 常规链路 数据长度:6 10Hz 对应机器人可接收(除工程)
    ROBOT_RFID_STATE_CMD_ID           = 0x0209,  //机器人RFID状态 常规链路 数据长度:5 3Hz 装RFID机器人可接收
    DART_CLIENT_CMD_ID                = 0x020A,  //飞镖机器人客户端指令数据 常规链路 数据长度:6 3Hz 飞镖可接收
    GROUND_ROBOT_POSITION_CMD_ID      = 0x020B,  //地面机器人位置数据 常规链路 数据长度:40 1Hz 哨兵可接收
    LIDAR_PROGRESS_CMD_ID             = 0x020C,  //雷达标记进度数据 常规链路 数据长度:2 1Hz 雷达可接收
    SENTRY_INFO_CMD_ID                = 0x020D,  //哨兵自主决策信息同步 常规链路 数据长度:6 1Hz 哨兵可接收
    LIDAR_INFO_CMD_ID                 = 0x020E,  //雷达自主决策信息同步 常规链路 数据长度:1 1Hz 雷达可接收


    STUDENT_INTERACTIVE_DATA_CMD_ID   = 0x0301,  //机器人交互数据 常规链路 数据长度:118 发送方触发,最大30Hz
    CUSTOM_CONTROLLER_DATA_CMD_ID     = 0x0302,  //自定义控制器与机器人交互数据 图传链路 数据长度:30 发送方触发,最大30Hz 自定义控制器对应客户端绑定机器人接收
    ROBOT_COMMAND_CMD_ID              = 0x0303,  //小地图下发信息标识 常规链路 数据长度:15 选手端触发发送 选手端选择的机器人接收
    CLIENT_MAP_COMMAND_CMD_ID         = 0x0305,  //小地图接收雷达信息标识 常规链路 数据长度:48 上限5Hz 所有选手端可接收
    CLIENT_CUSTOM_CONTROLLER_DATA_CMD_ID = 0x0306, //自定义控制器与选手端交互数据(使用自定义控制器模拟键鼠)  数据长度:8 发送方触发,最大30Hz 自定义控制器对应选手端接收
    CLIENT_MAP_PATH_DATA_CMD_ID       = 0x0307,  //选手端小地图接受路径数据 常规链路 数据长度:103 最大1Hz 哨兵选手客户端可接收
    CLIENT_ROBOT_INFO_CMD_ID          = 0x0308,  //选手端小地图接受机器人信息 常规链路 数据长度:34 最大3Hz 所有选手端可接收
    CUSTOM_CONTROLLER_RECEIVED_DATA_CMD_ID = 0x0309, //自定义控制器接收机器人数据 图传链路 数据长度:30 最大10Hz 自定义控制器接收
    ROBOT_CUSTOM_CLIENT_DATA_CMD_ID   = 0x310,   //机器人发送给自定义客户端的数据 图传链路 数据长度:300 最大50Hz 自定义客户端接收
    CUSTOM_CLIENT_ROBOT_DATA_CMD_ID   = 0x0311,  //自定义客户端发送给机器人的数据 图传链路 数据长度:30 75Hz 对应机器人接收
    SET_VIDEO_TRANSMIT_CHANNEL_CMD_ID = 0x0F01,  //设置图传出图信道 图传链路 数据长度: 发1收1
    QUERY_VIDEO_TRANSMIT_CHANNEL_CMD_ID = 0xF02, //查询当前出图信道 图传链路 数据长度: 发0收1 最大2Hz
    //剩下几个雷达无线链路的数据由于下位机不用，所以不做定义
    IDCustomData,   //用来标记enum的结束
}referee_cmd_id_e;

typedef enum {
    //机器人交互数据的子命令(0x301)
    UI_LAYER_DELETE_SUBCMD_ID      = 0x0100,      //选手端删除图层
    UI_CREATE_ONE_SUBCMD_ID        = 0x0101,      //选手端绘制一个图形
    UI_CREATE_TWO_SUBCMD_ID        = 0x0102,      //选手端绘制两个图形
    UI_CREATE_FIVE_SUBCMD_ID       = 0x0103,      //选手端绘制五个图形
    UI_CREATE_SEVEN_SUBCMD_ID      = 0x0104,      //选手端绘制七个图形
    UI_CREATE_CHARACTER_SUBCMD_ID  = 0x0110,      //选手端绘制字符图形

    DECISION_SENTRY_SUBCMD_ID                = 0x0120,      //哨兵自主决策指令
    DECISION_LIDAR_SUBCMD_ID                 = 0x0121,      //雷达自主决策指令
    ROBOT_INTERACTION_SUBCMD_ID              = 0x0200       //机器人之间通信指令 手册上设置id为0x0200~0x02FF,这里取0x0200

}referee_subcmd_id_e;




typedef struct _packed
{
    //Command 0x0001 比赛状态数据
    uint8_t game_type :4;     //1:对抗赛 2:单项赛 3:人工智能赛 4:联盟赛 5:步兵对抗赛
    uint8_t game_progress :4;  //0:未开始 1:准备阶段 2:自检阶段 3:5秒倒计时 4:比赛中 5:比赛结算中
    uint16_t stage_remaining_time; //当前阶段剩余时间，单位秒
    uint64_t SyncTimeStamp;  //UNIX时间，当机器人正确连接到裁判系统的NTP服务器后生效
}game_status_t;

typedef struct _packed
{   //Command 0x0002 比赛结果数据
    uint8_t winner;  //0:平局 1:红方获胜 2:蓝方获胜
}game_result_t;

typedef struct _packed
{   //Command 0x0003  机器人血量数据
    uint16_t ally_1_robot_HP; //己方1号机器人血量
    uint16_t ally_2_robot_HP; //己方2号机器人血量
    uint16_t ally_3_robot_HP; //己方3号机器人血量
    uint16_t ally_4_robot_HP; //己方4号机器人血量
    uint16_t reserved1;
    uint16_t ally_7_robot_HP; //己方7号机器人血量
    uint16_t ally_outpost_HP; //己方前哨站血量
    uint16_t ally_base_HP; //己方基地血量
}game_robot_HP_t;

typedef struct _packed
{   //Command 0x0101  场地事件数据
    uint32_t event_data;
}event_data_t;

typedef struct _packed
{   //Command 0x0104 裁判警告信息
    uint8_t level;  //受罚等级 1:双方黄牌 2.黄牌 3.红牌 4.判负
    uint8_t offending_robot_id; //己方受罚机器人ID，判负和双方黄牌值为0
    uint8_t count; //己方最后一次受到判罚的违规机器人对应判罚等级的违规次数
}referee_warning_t;

typedef struct _packed
{   //Command 0x0105 飞镖发射相关数据
    uint8_t dart_remaining_time; //飞镖发射剩余时间，单位秒
    uint16_t dart_info;
}dart_info_t;

typedef struct _packed
{   //Command 0x0201  比赛机器人状态
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t current_HP;
    uint16_t maximum_HP;
    uint16_t shooter_barrel_cooling_value;
    uint16_t shooter_barrel_heat_limit;
    uint16_t chassis_power_limit;
    uint8_t power_management_gimbal_output : 1;
    uint8_t power_management_chassis_output : 1;
    uint8_t power_management_shooter_output : 1;
}robot_status_t;

typedef struct _packed
{   //Command 0x0202  实时功率热量数据
    uint16_t reserved1;
    uint16_t reserved2;
    float reserved3;
    uint16_t buffer_energy;
    uint16_t shooter_17mm_1_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;
}power_heat_data_t;

typedef struct _packed
{   //Command 0x0203  机器人位置
    float x;
    float y;
    float angle;
}robot_pos_t;

typedef struct _packed
{   //Command 0x0204  机器人增益
    uint8_t recovery_buff;
    uint16_t cooling_buff;
    uint8_t defence_buff;
    uint8_t vulnerability_buff;
    uint16_t attack_buff;
    uint8_t remaining_energy;
}buff_t;

typedef struct _packed
{   //Command 0x0206  伤害状态
    uint8_t armor_id : 4;
    uint8_t HP_deduction_reason : 4;
}hurt_data_t;

typedef struct _packed
{   //Command 0x0207  实时射击信息
    uint8_t bullet_type;
    uint8_t shooter_number;
    uint8_t launching_frequency;
    float initial_speed;
}shoot_data_t;

typedef struct _packed
{   //Command 0x0208  子弹剩余发射数
    uint16_t projectile_allowance_17mm;
    uint16_t projectile_allowance_42mm;
    uint16_t remaining_gold_coin;
    uint16_t projectile_allowance_fortress;
}projectile_allowance_t;

typedef struct _packed
{   //Command 0x0209  机器人RFID状态
    uint32_t rfid_status;
    uint8_t  rfid_status_2;
}rfid_status_t;

typedef struct _packed
{   //Command 0x020A  飞镖机器人客户端指令数据
    uint8_t dart_launch_opening_status;
    uint8_t reserved1;
    uint16_t target_change_time;
    uint16_t latest_launch_cmd_time;
}dart_client_cmd_t;

typedef struct _packed
{   //Command 0x020B 地面机器人位置数据
    float hero_x;
    float hero_y;
    float engineer_x;
    float engineer_y;
    float standard_3_x;
    float standard_3_y;
    float standard_4_x;
    float standard_4_y;
    float reserved1;
    float reserved2;
}ground_robot_position_t;

typedef struct _packed
{   //Command 0x020C  雷达标记进度数据
    uint16_t mark_progress;
}radar_mark_data_t;

typedef struct _packed
{   //Command 0x020D  哨兵自主决策信息同步
    uint32_t sentry_info;
    uint16_t sentry_info_2;
} sentry_info_t;

typedef struct _packed
{   //Command 0x020E  雷达自主决策信息同步
    uint8_t radar_info;
} radar_info_t;



//TODO:
//虽然这里写了手册上定义的结构体，但是在dev_referee_ui重新定义了一套结构体
//下次得重构一下QAQ
typedef struct _packed
{   //Command 0x0301  机器人交互数据帧头
    uint16_t data_cmd_id;
    uint16_t sender_id;
    uint16_t receiver_id;
}robot_interaction_header_t;






//机器人交互帧子协议(UI)
typedef struct _packed
{   //SubCommand 0x0100
    uint8_t delete_type;
    uint8_t layer;
}interaction_layer_delete_t;

typedef struct _packed
{   //SubCommand 0x0101
    uint8_t figure_name[3];
    uint32_t operate_type:3;
    uint32_t figure_type:3;
    uint32_t layer:4;
    uint32_t color:4;
    uint32_t details_a:9;
    uint32_t details_b:9;
    uint32_t width:10;
    uint32_t start_x:11;
    uint32_t start_y:11;
    uint32_t details_c:10;
    uint32_t details_d:11;
    uint32_t details_e:11;
}interaction_figure_t;

typedef struct _packed
{   //SubCommand 0x0102
    interaction_figure_t interaction_figure[2];
}interaction_figure_2_t;

typedef struct _packed
{   //SubCommand 0x0103
    interaction_figure_t interaction_figure[5];
}interaction_figure_3_t;

typedef struct _packed
{   //SubCommand 0x0104
    interaction_figure_t interaction_figure[7];
}interaction_figure_4_t;

typedef struct _packed
{   //SubCommand 0x0110
    uint8_t  string_name[3];
    uint32_t operate_type:3;
    uint32_t graphic_type:3;
    uint32_t layer:4;
    uint32_t color:4;
    uint32_t details_a:9;
    uint32_t details_b:9;
    uint32_t width:10;
    uint32_t start_x:11;
    uint32_t start_y:11;
    uint32_t details_c:10;
    uint32_t details_d:11;
    uint32_t details_e:11;
    uint8_t stringdata[30];
} ext_client_custom_character_t;



typedef struct _packed
{   //SubCommand 0x0120
    uint32_t sentry_cmd;
} sentry_cmd_t;

typedef struct _packed
{   //SubCommand 0x0121
    uint8_t radar_cmd;
    uint8_t password_cmd;
    uint8_t password_1;
    uint8_t password_2;
    uint8_t password_3;
    uint8_t password_4;
    uint8_t password_5;
    uint8_t password_6;
} radar_cmd_t;


typedef struct _packed
{   //Command 0x0302  自定义控制器与机器人交互数据(机器人接收)
    uint8_t data[30];
}custom_robot_data_t;


typedef struct _packed
{   //Command 0x0303  小地图下发信息标识
    float target_position_x;
    float target_position_y;
    uint8_t cmd_keyboard;
    uint8_t target_robot_id;
    uint16_t cmd_source;
}map_command_t;

//Deprecated API
// typedef struct _packed
// {  //Command 0x0304  键鼠数据
//     int16_t mouse_x;
//     int16_t mouse_y;
//     int16_t mouse_z;
//     int8_t left_button_down;
//     int8_t right_button_down;
//     uint16_t keyboard_value;
//     uint16_t reserved;
// }remote_control_t;

//选手端接收数据
typedef struct _packed
{   //Command 0x0305  小地图接收信息标识
    uint16_t opponent_hero_position_x;
    uint16_t opponent_hero_position_y;
    uint16_t opponent_engineer_position_x;
    uint16_t opponent_engineer_position_y;
    uint16_t opponent_infantry_3_position_x;
    uint16_t opponent_infantry_3_position_y;
    uint16_t opponent_infantry_4_position_x;
    uint16_t opponent_infantry_4_position_y;
    uint16_t opponent_aerial_position_x;
    uint16_t opponent_aerial_position_y;
    uint16_t opponent_sentry_position_x;
    uint16_t opponent_sentry_position_y;
    uint16_t ally_hero_position_x;
    uint16_t ally_hero_position_y;
    uint16_t ally_engineer_position_x;
    uint16_t ally_engineer_position_y;
    uint16_t ally_infantry_3_position_x;
    uint16_t ally_infantry_3_position_y;
    uint16_t ally_infantry_4_position_x;
    uint16_t ally_infantry_4_position_y;
    uint16_t ally_aerial_position_x;
    uint16_t ally_aerial_position_y;
    uint16_t ally_sentry_position_x;
    uint16_t ally_sentry_position_y;
} map_robot_data_t;

typedef struct _packed
{   //Command 0x0306  自定义控制器与选手端交互数据
    uint16_t key_value;
    uint16_t x_position:12;
    uint16_t mouse_left:4;
    uint16_t y_position:12;
    uint16_t mouse_right:4;
    uint16_t reserved;
}custom_client_data_t;

typedef struct _packed
{   //Command 0x0307 选手端小地图接受路径数据
    uint8_t intention;
    uint16_t start_position_x;
    uint16_t start_position_y;
    int8_t delta_x[49];
    int8_t delta_y[49];
    uint16_t sender_id;
}map_data_t;

typedef struct _packed
{   //Command 0x0308 选手端小地图接受机器人信息
    uint16_t sender_id;
    uint16_t receiver_id;
    uint8_t user_data[30];
} custom_info_t;

typedef struct _packed
{   //Command 0x0309  自定义控制器接收机器人数据(自定义控制器接收)
    uint8_t  data[30];
}robot_custom_data_t;

typedef struct _packed
{   //Command 0x0310  机器人向自定义客户端发送数据
    uint8_t  data[300];
}robot_custom_data_2_t;

typedef struct _packed
{   //Command 0x0311  自定义客户端发送给机器人数据
    uint8_t  data[30];
}custom_client_robot_data_t;


#endif //DEV_REFEREE_PROTOCOL_H
