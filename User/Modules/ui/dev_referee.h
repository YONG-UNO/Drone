/**
 * @file dev_referee.h
 * @author Ma HuaCheng
 * @brief 裁判系统通信模块
 * @version 0.2
 * @details 提供裁判系统数据的接收与解析功能(该代码基于RoboMaster裁判系统串口协议V1.3.0 2026.3 进行开发)
 * @date 2025-10-10
 * @update 2026-3-28
 * @copyright  Copyright (c) 2026 HDU—PHOENIX
 * @todo
 */
#ifndef DEV_REFEREE_H
#define DEV_REFEREE_H

#include <stdint.h>
#include "dev_referee_protocol.h"
#include "bsp_uart.h"
#include "usart.h"
#include "dev_daemon.h"



#define REF_CIR_BUFFER_SIZE 512  //环形缓冲区大小(目前版本环形缓冲区未实装)
#define REF_DOUBLE_RX_BUFFER_SIZE 256 //双缓冲区的一个缓冲区的大小
#define REF_UART_IT_RX_LEN 1   //IT模式一次接受消费的数据长度

typedef struct {
    uint8_t buffer[REF_CIR_BUFFER_SIZE];
    size_t head;
    size_t tail;
    size_t count;
} RefereeCircularBuffer_t;

void RefereeCircularBuffer_Init(RefereeCircularBuffer_t *cb);
bool RefereeCircularBuffer_Put(RefereeCircularBuffer_t *cb, const uint8_t *data, size_t len);
size_t RefereeCircularBuffer_Get(RefereeCircularBuffer_t *cb, uint8_t *out, size_t max_len);
bool RefereeCircularBuffer_IsEmpty(const RefereeCircularBuffer_t *cb);
size_t RefereeCircularBuffer_Available(const RefereeCircularBuffer_t *cb);


//裁判系统内容
#define   REF_HEADER_SOF 0xA5 //帧头起始字节
#define   REF_PROTOCOL_HEADER_SIZE 	  5				//帧头长
#define   REF_PROTOCOL_CRC16_SIZE     2       //命令码长度
#define   REF_PROTOCOL_TAIL_SIZE      2	      //帧尾CRC16

#define REF_PROTOCOL_FRAME_MAX_SIZE 128
#define REF_HEADER_CRC_LEN          (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE)
#define REF_HEADER_CRC_CMDID_LEN    (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE + sizeof(uint16_t))
#define REF_HEADER_CMDID_LEN        (REF_PROTOCOL_HEADER_SIZE + sizeof(uint16_t))


typedef struct _packed {
    //帧头格式
    uint8_t SOF;
    uint16_t Data_Length;
    uint8_t Seq;
    uint8_t CRC8;
} frame_header_t;

//解包数据暂存结构体
typedef struct _packed {
    uint16_t data_len;
    uint8_t protocol_packet[REF_PROTOCOL_FRAME_MAX_SIZE];
    uint16_t index;
    referee_cmd_id_e cmd_id;
} Referee_unpack_data_s;


typedef struct _packed {
    //解析完的原始结构体变量，可以在此获取原始裁判系统数据
    game_status_t ext_game_status;
    game_result_t ext_game_result;
    game_robot_HP_t ext_game_robot_HP;
    event_data_t ext_event_data;
    referee_warning_t ext_referee_warning;
    dart_info_t ext_dart_info;
    robot_status_t ext_robot_status;
    power_heat_data_t ext_power_heat_data;
    robot_pos_t ext_robot_pos;
    buff_t ext_buff;
    hurt_data_t ext_hurt_data;
    shoot_data_t ext_shot_data;
    projectile_allowance_t ext_projectile_allowance;
    rfid_status_t ext_rfid_status;
    dart_client_cmd_t ext_dart_cmd;
    ground_robot_position_t ext_ground_robot_position;
    radar_mark_data_t ext_radar_mark_data;
    sentry_info_t ext_sentry_info;
    radar_info_t ext_radar_info;
    custom_client_robot_data_t ext_custom_client_robot_data_t;
    map_command_t ext_map_command;
    custom_robot_data_t ext_custom_robot_data;
    robot_custom_data_t ext_robot_custom_data;


    //TODO: 0x301机器人交互帧需要特殊处理,下面是子协议的变量
    //0x301机器人交互帧子协议
    sentry_cmd_t ext_sentry_cmd;
    radar_cmd_t ext_radar_cmd;
} Referee_Origin_data_s;

//VT03接收遥控器数据(该协议脱离于裁判系统，仅当rx_size=21时触发)
//结构体大小: 2+8+6+1+4 = 21Byte
typedef struct __attribute__((packed)) VT03_Rx_Message_t_
{
    uint8_t sof_1;
    uint8_t sof_2;
    uint64_t ch_0 : 11;
    uint64_t ch_1 : 11;
    uint64_t ch_2 : 11;
    uint64_t ch_3 : 11;
    uint64_t mode_sw : 2;
    uint64_t pause : 1;
    uint64_t fn_1 : 1;
    uint64_t fn_2 : 1;
    uint64_t wheel : 11;
    uint64_t trigger : 1;
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;
    uint8_t mouse_left : 2;
    uint8_t mouse_right : 2;
    uint8_t mouse_middle : 2;
    uint16_t key;
    uint16_t crc16;
} VT03_Rx_Message_t;

typedef struct {
    char *topic_name;
    UART_HandleTypeDef *uart_handle;
    UartMode_e mode;  // 支持中断模式与dma模式,dma模式下会开启环形缓冲区效率更高
    Daemon_InitConfig_s* daemon_config;
} RefereeInitConfig_s;

typedef struct {
    char *topic_name;
    UartInstance_s *uart_instance;
    Daemon_Instance_s* daemon_instance;
    VT03_Rx_Message_t vt03_data;
    Referee_Origin_data_s origin_data;
    bool Referee_Data_TF; //当前裁判系统数据是否就绪
    uint8_t Referee_Self_ID; //当前机器人的ID
    uint16_t Referee_SelfClient_ID; //发送者机器人对应的客户端ID
    uint32_t custom_robot_update_time ;
    uint32_t cnt;
    uint32_t rx_freq;
} RefereeInstance_s;

//裁判系统接收解包状态机
typedef enum {
    STEP_HEADER_SOF = 0, //在搜索帧头SOF ing
    STEP_LENGTH_LOW = 1, //在解析帧长度低字节ing
    STEP_LENGTH_HIGH = 2, //在解析帧长度高字节ing
    STEP_FRAME_SEQ = 3, //在解析帧序号ing
    STEP_HEADER_CRC8 = 4, //在解析帧头CRC检验数ing
    STEP_DATA_CRC16 = 5, //解析数据帧ing
} Referee_Rx_StatusMachine_e;




/**
 * @brief 裁判系统注册函数
 * @param config 初始化参数
 * @return 裁判系统实例指针
 */
RefereeInstance_s *Referee_Register(const RefereeInitConfig_s *config);

/**
 * @brief 裁判系统数据解包函数
 * @param ref_instance 裁判系统实例
 * @param data 原始数据
 * @note 该函数为内部函数，外部无需调用
 */
void Referee_Decode_unpack_data(RefereeInstance_s *ref_instance, const uint8_t *data);

/**
 * @brief 裁判系统数据初始化函数
 * @param ref_instance 裁判系统实例
 * @note 该函数为内部函数，外部无需调用
 */
void Referee_Data_Init(RefereeInstance_s *ref_instance);

/**
 * @brief 向裁判系统发送数据
 * @param ref_instance 裁判系统实例
 * @param cmd_id 命令 ID
 * @param data_length 数据长度
 * @param seq 包序号
 * @param data 数据指针
 * @return 是否发送成功
 */
bool Referee_Send_Msg(RefereeInstance_s* ref_instance,referee_cmd_id_e cmd_id,uint16_t data_length,uint8_t seq, uint8_t *data );



//辅助函数，从当前已解包裁判系统数据结构体中提取具体数据

/**
 * @brief 获取裁判系统数据状态
 * @param ref_instance 裁判系统实例
 * @return 数据是否就绪 false:未就绪 true:就绪
 */
bool Referee_Get_Data_Status(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取队伍颜色(红蓝)
 * @param ref_instance 裁判系统实例
 * @return 颜色为 0:蓝队 1:红队
 */
bool Referee_Get_Color(RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取机器人ID
 * @param ref_instance 裁判系统实例
 * @return 机器人ID,详情ID代表的意思可见referee_protocol.h
 */
uint8_t Referee_Get_Robot_ID(RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取客户端ID
 * @param ref_instance 裁判系统实例
 * @return 客户端ID,详情ID代表的意思可见referee_protocol.h
 */
uint16_t Referee_Get_Client_ID(RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取当前比赛阶段
 * @param ref_instance 裁判系统实例
 * @return 比赛阶段 0:未开始 1:准备阶段 2:自检阶段 3:5秒倒计时 4:比赛中 5:比赛结算中
 */
uint8_t Referee_Get_Game_Status(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取机器人当前底盘功率限制
 * @param ref_instance 裁判系统实例
 * @return 机器人当前底盘功率限制
 */
uint16_t Referee_Get_Power_Limit(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取机器人当前剩余能量(底盘能量耗尽后会导致底盘功率上限大幅缩减)
 * @param ref_instance 裁判系统实例
 * @return 机器人剩余当前能量值返回，以16进制标识机器人剩余能量值比例，仅在剩余能量小于50%时反馈，其余默认反馈0x32
 * bit0: 在剩余能量>=50%时为1,其余情况为0
 * bit1：在剩余能量≥30%时为 1，其余情况为 0
 * bit2：在剩余能量≥15%时为 1，其余情况为 0
 * bit3：在剩余能量≥5%时为 1，其余情况为 0
 * Bit4：在剩余能量≥1%时为 1，其余情况为 0
 */
uint16_t Referee_Get_Remain_Energy(const RefereeInstance_s *ref_instance);


/**
 * @brief 裁判系统获取机器人当前剩余缓冲能量(缓冲能量耗尽且底盘功率超限会导致底盘断电)
 * @param ref_instance 裁判系统实例
 * @return 缓冲能量（单位：J）
 */
uint16_t Referee_Get_Buffer_Energy(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取机器人等级
 * @param ref_instance 裁判系统实例
 * @return 机器人当前等级
 */
uint8_t Referee_Get_Robot_Level(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取当前42mm发射机构热量
 * @param ref_instance 裁判系统实例
 * @return 当前42mm发射机构热量
 */
uint16_t Referee_Get_Shooter_Heat_42(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取当前17mm发射机构热量
 * @param ref_instance 裁判系统实例
 * @return 当前17mm发射机构热量
 */
uint16_t Referee_Get_Shooter_Heat_17(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取当前机器人发射机构热量上限
 * @param ref_instance 裁判系统实例
 * @return 当前机器人发射机构热量上限
 */
uint16_t Referee_Get_Heat_Limit(const RefereeInstance_s *ref_instance);

/**
 * @biref 裁判系统获取当前机器人发射(上一发)弹丸初速度
 * @param ref_instance 裁判系统实例
 * @return 当前机器人发射弹丸初速度
 */
float Referee_Get_Shooter_Speed(const RefereeInstance_s *ref_instance);

/**
 * @brief 裁判系统获取当前机器人发射机构射击热量每秒冷却值
 * @param ref_instance 裁判系统实例
 * @return 当前机器人发射机构裁判系统实例
 */
uint16_t Referee_Get_Shooter_Cold(const RefereeInstance_s *ref_instance);




//发送辅助函数

/**
 * @brief 向裁判系统发送自定义控制器数据(自定义控制器->对应图传连接机器人) 0x302
 * @param ref_instance 裁判系统实例
 * @param data 自定义控制器自定义数据
 * @param length 自定义数据长度
 * @return 发送结果 true:发送成功 false:发送失败
 */
bool Referee_Send_Custom_Msg_To_Robot(RefereeInstance_s *ref_instance,uint8_t *data, uint16_t length);

/**
 * @brief 向裁判系统发送机器人数据(机器人->自定义控制器) 0x309
 * @param ref_instance 裁判系统实例
 * @param data 自定义控制器自定义数据
 * @param length 自定义数据长度
 * @return 发送结果 true:发送成功 false:发送失败
 */
bool Referee_Send_Robot_Msg_To_Controller(RefereeInstance_s *ref_instance,uint8_t *data, uint16_t length);

/**
 * @brief 向裁判系统发送机器人数据(机器人->自定义客户端) 0x310
 * @param ref_instance 裁判系统实例
 * @param data 自定义控制器自定义数据
 * @param length 自定义数据长度
 * @return 发送结果 true:发送成功 false:发送失败
 * @note 考虑到发送长度大于bsp_uart的发送长度，如需使用这个函数请自行更改bsp_uart的tx_buff长度
 */
bool Referee_Send_Robot_Msg_To_Custom_Client(RefereeInstance_s *ref_instance,uint8_t *data, uint16_t length);

/**
 * @brief 清除裁判系统绑定的串口错误(包括FrameError,OverRunError,NoiseError)
 * @param ref_instance 裁判系统实例
 */
void Referee_Clear_Uart_Error(RefereeInstance_s *ref_instance);
#endif //DEV_REFEREE_H
