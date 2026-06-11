/**
 * @file dev_referee.c
 * @author Ma HuaCheng
 * @brief 裁判系统通信模块
 * @version 0.2
 * @details 提供裁判系统数据的接收与解析功能(该代码基于RoboMaster裁判系统串口协议V1.3.0 2026.3 进行开发)
 * @date 2025-10-10
 * @update 2026-3-28
 * @copyright  Copyright (c) 2026 HDU—PHOENIX
 * @todo
 */

#include "dev_referee.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "projdefs.h"

#include "FreeRTOSConfig.h"

#include <string.h>

#include "alg_crc.h"
//双缓冲区
uint8_t active_rx_buff = 0;
uint8_t ref_rx_Buffer[2][REF_DOUBLE_RX_BUFFER_SIZE] = {0};

Referee_Rx_StatusMachine_e status = 0; //当前解包状态机
Referee_unpack_data_s unpack_data_buffer; // 当前解包实时存储的数据
frame_header_t unpack_frame_header; //解包时存储当前帧头
uint8_t referee_tx_buffer[310]; //最长可发送包长度 5+2+300+2+1

static TickType_t last_freq_calc_time = 0;  // 上次频率计算时间
static uint32_t last_cnt = 0;
uint16_t vt03_cnt = 0;

//流式解包函数(因为裁判系统每帧长度不一且可能发生粘包需要流式解包)
bool Referee_Uart_Handle_One_Byte(RefereeInstance_s* instance, uint8_t byte)
{
    switch (status)
    {
    case STEP_HEADER_SOF:
        if (byte == REF_HEADER_SOF)
        {
            status = STEP_LENGTH_LOW;
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
        }
        else
        {
            // Log_Debug("Get unexpected data %02X , index = %d ", byte , unpack_data_buffer.index);
            unpack_data_buffer.index = 0;
            unpack_data_buffer.data_len = 0;

        }
        break;

    case STEP_LENGTH_LOW:
        {
            unpack_data_buffer.data_len = byte;
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            status = STEP_LENGTH_HIGH;
        }
        break;

    case STEP_LENGTH_HIGH:
        {
            unpack_data_buffer.data_len |= (byte << 8);
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            //如果解析长度大于最大长度，则废弃
            if (unpack_data_buffer.data_len < (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_LEN))
            {
                status = STEP_FRAME_SEQ;
            }
            else
            {
                status = STEP_HEADER_SOF;
                unpack_data_buffer.index = 0;
            }
        }
        break;

    case STEP_FRAME_SEQ:
        {
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            status = STEP_HEADER_CRC8;
        }
        break;

    case STEP_HEADER_CRC8:
        {
            unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte;
            if (unpack_data_buffer.index == REF_PROTOCOL_HEADER_SIZE)
            {
                if (CRC08_Calculate(unpack_data_buffer.protocol_packet, unpack_data_buffer.index - 1) == byte)
                {
                    status = STEP_DATA_CRC16;
                }
                else
                {
                    status = STEP_HEADER_SOF;
                    unpack_data_buffer.index = 0;
                }
            }
        }
        break;

    case STEP_DATA_CRC16:
        {
        unpack_data_buffer.protocol_packet[unpack_data_buffer.index++] = byte; // 先存入
        if (unpack_data_buffer.index >= (REF_HEADER_CRC_CMDID_LEN + unpack_data_buffer.data_len))
        {
            // 一帧完整结束，状态机与缓存区刷新
            status = STEP_HEADER_SOF;
            unpack_data_buffer.index = 0;
            if (CRC16_Verify(unpack_data_buffer.protocol_packet, REF_HEADER_CRC_CMDID_LEN + unpack_data_buffer.data_len))
            {
                Referee_Decode_unpack_data(instance, unpack_data_buffer.protocol_packet);
                instance->cnt++;

                TickType_t current_time = xTaskGetTickCount();
                TickType_t time_diff = current_time - last_freq_calc_time;
                if (time_diff >= pdMS_TO_TICKS(1000)) {
                    uint32_t cnt_diff = instance->cnt - last_cnt;
                    instance->rx_freq = (cnt_diff * configTICK_RATE_HZ) / time_diff;
                    last_freq_calc_time = current_time;
                    last_cnt = instance->cnt;
                }
                return 1;
            }
            else
            {
            }
        }
        break;
        }
    default:
        {
             instance->Referee_Data_TF = false;
            status = STEP_HEADER_SOF;
            unpack_data_buffer.index = 0;
        }
        break;
    }
    return 0;
}

//中断模式回调
static void Referee_Uart_Callback(UartInstance_s* instance)
{
    for (int i =0; i < instance->rx_len; i++) {
        uint8_t byte = instance->rx_buff[i];
        Referee_Uart_Handle_One_Byte(((RefereeInstance_s*)instance->id),byte);
    }
    HAL_UART_Receive_IT(instance->uart_handle,instance->rx_buff,instance->rx_len);
}

//IDLE模式回调
static void Referee_Uart_DMA_Callback(UartInstance_s* instance)
{
    //双缓冲区接收
    uint8_t ready_rx_buff = active_rx_buff;
    active_rx_buff = active_rx_buff == 0 ? 1 : 0;
    //立刻重开dma
    HAL_UARTEx_ReceiveToIdle_DMA(instance->uart_handle,ref_rx_Buffer[active_rx_buff],REF_DOUBLE_RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(instance->uart_handle->hdmarx, DMA_IT_HT);

    // Log_Debug("Callback Trigger, rx_size = %d", instance->rx_size);
    //消费数据
    //如果长度=21则检验是否为vt03数据
    if (instance->rx_size ==21)
    {
        //判断帧头
        if (ref_rx_Buffer[ready_rx_buff][0] == 0xA9 && ref_rx_Buffer[ready_rx_buff][1] == 0x53)
        {
           if (CRC16_Verify(ref_rx_Buffer[ready_rx_buff],21))
           {
               memcpy(&((RefereeInstance_s*)instance->id)->vt03_data,ref_rx_Buffer[ready_rx_buff],21);
               vt03_cnt++;
               return;
           }
        }
    }

    //如果长度不是21但帧头是vt03则检查vt03后面有没有粘包裁判系统数据
    if (ref_rx_Buffer[ready_rx_buff][0] == 0xA9 && ref_rx_Buffer[ready_rx_buff][1] == 0x53) {
        if (instance->rx_size >21 && ref_rx_Buffer[ready_rx_buff][21] == REF_HEADER_SOF) {
            for (int i = 21; i < instance->rx_size; i++){
                Referee_Uart_Handle_One_Byte(((RefereeInstance_s*)instance->id),ref_rx_Buffer[ready_rx_buff][i]);
            }
        }
        return;
    }

    //如果当前rxbuffer首字节是帧头则重置状态机
    if (ref_rx_Buffer[ready_rx_buff][0] == REF_HEADER_SOF ) {
        status = STEP_HEADER_SOF;
        unpack_data_buffer.index = 0;
        unpack_data_buffer.data_len = 0;
    }

    for (int i =0; i < instance->rx_size; i++) {
        // Log_Debug("Processing data %02X , i = %d",ref_rx_Buffer[ready_rx_buff][i], i);
        //收到完整一帧若后续粘包了vt03消息过滤掉
        if (Referee_Uart_Handle_One_Byte(((RefereeInstance_s*)instance->id),ref_rx_Buffer[ready_rx_buff][i]) == 1)
        {
            if (i+2 < instance->rx_size && ref_rx_Buffer[ready_rx_buff][i+1] == 0xA9 && ref_rx_Buffer[ready_rx_buff][i+2] == 0x53)
            {
                return;
            }
        }
    }
    // Log_Debug("callback finished, rx_size = %d", instance->rx_size);  // 确认循环完整执行
}


//数据帧解析函数
void Referee_Decode_unpack_data(RefereeInstance_s* ref_instance, const uint8_t* data)
{
    uint8_t index = 0;
    if (ref_instance->daemon_instance != NULL)
    {
        Daemon_Feed(ref_instance->daemon_instance);
    }
    memcpy(&unpack_frame_header, data, sizeof(frame_header_t));
    index += sizeof(frame_header_t);
    //命令解析
    memcpy(&(unpack_data_buffer.cmd_id), data + index, sizeof(uint16_t));
    index += sizeof(uint16_t);
    switch (unpack_data_buffer.cmd_id)
    {
    case GAME_STATE_CMD_ID: memcpy(&(ref_instance->origin_data.ext_game_status), data + index, sizeof(game_status_t));
        break;
    case GAME_RESULT_CMD_ID: memcpy(&(ref_instance->origin_data.ext_game_result), data + index, sizeof(game_result_t));
        break;
    case GAME_ROBOT_HP_CMD_ID: memcpy(&(ref_instance->origin_data.ext_game_robot_HP), data + index, sizeof(game_robot_HP_t));
        break;
    case FIELD_EVENTS_CMD_ID: memcpy(&(ref_instance->origin_data.ext_event_data), data + index, sizeof(event_data_t));
        break;
    case REFEREE_WARNING_CMD_ID: memcpy(&(ref_instance->origin_data.ext_referee_warning), data + index, sizeof(referee_warning_t));
        break;
    case DART_REMAINING_TIME_CMD_ID: memcpy(&(ref_instance->origin_data.ext_dart_info), data + index, sizeof(dart_info_t));
        break;
    case ROBOT_STATE_CMD_ID: memcpy(&(ref_instance->origin_data.ext_robot_status), data + index, sizeof(robot_status_t));
        break;
    case POWER_HEAT_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_power_heat_data), data + index, sizeof(power_heat_data_t));
        break;
    case ROBOT_POS_CMD_ID: memcpy(&(ref_instance->origin_data.ext_robot_pos), data + index, sizeof(robot_pos_t));
        break;
    case BUFF_MUSK_CMD_ID: memcpy(&(ref_instance->origin_data.ext_buff), data + index, sizeof(buff_t));
        break;
    case ROBOT_HURT_CMD_ID: memcpy(&(ref_instance->origin_data.ext_hurt_data), data + index, sizeof(hurt_data_t));
        break;
    case SHOOT_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_shot_data), data + index, sizeof(shoot_data_t));
        break;
    case BULLET_REMAINING_CMD_ID: memcpy(&(ref_instance->origin_data.ext_projectile_allowance), data + index, sizeof(projectile_allowance_t));
        break;
    case ROBOT_RFID_STATE_CMD_ID: memcpy(&(ref_instance->origin_data.ext_rfid_status), data + index, sizeof(rfid_status_t));
        break;
    case DART_CLIENT_CMD_ID: memcpy(&(ref_instance->origin_data.ext_dart_cmd), data + index, sizeof(dart_client_cmd_t));
        break;
    case GROUND_ROBOT_POSITION_CMD_ID:
        memcpy(&(ref_instance->origin_data.ext_ground_robot_position), data + index, sizeof(ground_robot_position_t));
        break;
    case LIDAR_PROGRESS_CMD_ID: memcpy(&(ref_instance->origin_data.ext_radar_mark_data), data + index, sizeof(radar_mark_data_t));
        break;
    case SENTRY_INFO_CMD_ID: memcpy(&(ref_instance->origin_data.ext_sentry_info), data + index, sizeof(sentry_info_t));
        break;
    case LIDAR_INFO_CMD_ID: memcpy(&(ref_instance->origin_data.ext_radar_info), data + index, sizeof(radar_info_t));
        break;

    case ROBOT_COMMAND_CMD_ID: memcpy(&(ref_instance->origin_data.ext_map_command), data + index, sizeof(map_command_t));
        break;
    case CUSTOM_CONTROLLER_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_custom_robot_data), data + index, sizeof(custom_robot_data_t));
        ref_instance->custom_robot_update_time =xTaskGetTickCount();
        break;
    case CUSTOM_CONTROLLER_RECEIVED_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_robot_custom_data), data + index,sizeof(custom_robot_data_t));
        break;
    case CUSTOM_CLIENT_ROBOT_DATA_CMD_ID: memcpy(&(ref_instance->origin_data.ext_custom_client_robot_data_t), data + index, sizeof(custom_client_robot_data_t));
        break;
    default:
            break;
    }
    ref_instance->Referee_Data_TF = true;
    // Log("Get Referee frame");
}

RefereeInstance_s* Referee_Register(const RefereeInitConfig_s* config)
{
    if (config == NULL)
    {
        return NULL;
    }
RefereeInstance_s* instance = pvPortMalloc(sizeof(RefereeInstance_s));    if (instance == NULL)
    {
        return NULL;
    }

    UartInitConfig_s uart_config = {0};
    if (config->mode == UART_IT_MODE) {
        uart_config.id = instance;
        uart_config.mode = config->mode;
        uart_config.rx_len = REF_UART_IT_RX_LEN;
        uart_config.uart_module_callback = Referee_Uart_Callback;
        uart_config.uart_handle = config->uart_handle;
    }
    else if ( config->mode == UART_IDLE_MODE) {
        uart_config.id = instance;
        uart_config.mode = config->mode;
        uart_config.rx_len = 1;  //由于bsp_uart不支持512B的接收长度，这里改成1回调内重新512B接收
        uart_config.uart_module_callback = Referee_Uart_DMA_Callback;
        uart_config.uart_handle = config->uart_handle;
    }
    else {
    }

    //Uart初始化
    UartInstance_s* uart_instance = Uart_Register(&uart_config);
    if (uart_instance == NULL)
    {
    }
    instance->uart_instance = uart_instance;

    // Daemon初始化
    if (config->daemon_config != NULL)
    {
        Daemon_Instance_s* daemon_instance = Daemon_Register(config->daemon_config);
        if (daemon_instance == NULL)
        {
        }
        instance->daemon_instance = daemon_instance;
        Daemon_Start(instance->daemon_instance);
    }

    instance->topic_name = config->topic_name;
    instance->cnt = 0;
    instance->rx_freq = 0;
    instance->Referee_Data_TF = false;
    instance->custom_robot_update_time  = 0;
    Referee_Data_Init(instance);
    return instance;
}

void Referee_Data_Init(RefereeInstance_s* ref_instance) {
    memset(&(ref_instance->origin_data),0,sizeof(Referee_Origin_data_s));
}

bool Referee_Send_Msg(RefereeInstance_s* ref_instance, referee_cmd_id_e cmd_id,uint16_t data_length,uint8_t seq, uint8_t *data ) {
    if (ref_instance==NULL) {
        return false;
    }
    referee_tx_buffer[0] = REF_HEADER_SOF;
    referee_tx_buffer[1] = (uint8_t)(data_length & 0xFF);
    referee_tx_buffer[2] = (uint8_t)((data_length >> 8) & 0xFF);
    referee_tx_buffer[3] = seq;
    referee_tx_buffer[4] = CRC08_Calculate(referee_tx_buffer, 4);
    referee_tx_buffer[5] = (uint8_t)(cmd_id & 0xFF);
    referee_tx_buffer[6] = (uint8_t)((cmd_id >> 8) & 0xFF);
    memcpy(referee_tx_buffer+7,data,data_length);
    uint16_t crc16 = CRC16_Calculate(referee_tx_buffer, 7 +  data_length);
    referee_tx_buffer[7+data_length] = (uint8_t)(crc16 & 0xFF);
    referee_tx_buffer[7+data_length+1] = (uint8_t)((crc16 >> 8)&0xFF);
    return Uart_Transmit_Len(ref_instance->uart_instance, referee_tx_buffer,  data_length + 9);
}

bool Referee_Get_Data_Status(const RefereeInstance_s* ref_instance) {
    return ref_instance->Referee_Data_TF;
}

bool Referee_Get_Color(RefereeInstance_s* ref_instance) {
    ref_instance->Referee_Self_ID = ref_instance->origin_data.ext_robot_status.robot_id;
    if (ref_instance->origin_data.ext_robot_status.robot_id > 10) {
        return BLUE;
    }
    else {
        return RED;
    }
}

uint8_t Referee_Get_Robot_ID(RefereeInstance_s* ref_instance) {
    ref_instance->Referee_Self_ID = ref_instance->origin_data.ext_robot_status.robot_id;
    return ref_instance->Referee_Self_ID;
}

uint16_t Referee_Get_Client_ID(RefereeInstance_s* ref_instance) {
    bool color = Referee_Get_Color(ref_instance);
    if (color == BLUE) {
        ref_instance->Referee_SelfClient_ID = 0x0164+(ref_instance->Referee_Self_ID - 100);
    }
    else {
        ref_instance->Referee_SelfClient_ID = 0x0100 + ref_instance->Referee_Self_ID;
    }
    return ref_instance->Referee_SelfClient_ID;
}

uint8_t Referee_Get_Game_Status(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_game_status.game_progress;
}

uint16_t Referee_Get_Power_Limit(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.chassis_power_limit;
}

uint16_t Referee_Get_Remain_Energy(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_buff.remaining_energy ;
}

uint16_t Referee_Get_Buffer_Energy(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_power_heat_data.buffer_energy;
}

uint8_t Referee_Get_Robot_Level(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.robot_level;
}

uint16_t Referee_Get_Shooter_Heat_42(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_power_heat_data.shooter_42mm_barrel_heat;
}

uint16_t Referee_Get_Shooter_Heat_17(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_power_heat_data.shooter_17mm_1_barrel_heat;
}

uint16_t Referee_Get_Heat_Limit(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.shooter_barrel_heat_limit;
}

float Referee_Get_Shooter_Speed(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_shot_data.initial_speed;
}

uint16_t Referee_Get_Shooter_Cold(const RefereeInstance_s* ref_instance) {
    return ref_instance->origin_data.ext_robot_status.shooter_barrel_cooling_value;
}



bool Referee_Send_Custom_Msg_To_Robot( RefereeInstance_s *ref_instance,uint8_t *data , uint16_t length)
{
    //计数
    static int custom_robot_msg_cnt = 0;
    if (length > 30) {
        return false;
    }
    return Referee_Send_Msg(ref_instance, CUSTOM_CONTROLLER_DATA_CMD_ID, length, custom_robot_msg_cnt++, data);
}


bool Referee_Send_Robot_Msg_To_Controller( RefereeInstance_s *ref_instance,uint8_t *data,uint16_t length)
{
    //计数
    static int robot_custom_msg_cnt = 0;
    if (length > 32) {
        return false;
    }
    return Referee_Send_Msg(ref_instance, CUSTOM_CONTROLLER_RECEIVED_DATA_CMD_ID, length, robot_custom_msg_cnt++, data);
}


bool Referee_Send_Robot_Msg_To_Custom_Client(RefereeInstance_s *ref_instance,uint8_t *data, uint16_t length)
{
    //计数
    //考虑到发送长度大于bsp_uart的发送长度，如需使用这个函数请自行更改bsp_uart的tx_buff长度
    static int robot_custom_client_msg_cnt = 0;
    if (length > 300)
    {
        return false;
    }
    return Referee_Send_Msg(ref_instance, ROBOT_CUSTOM_CLIENT_DATA_CMD_ID, length, robot_custom_client_msg_cnt++, data);
}

void Referee_Clear_Uart_Error(RefereeInstance_s *ref_instance)
{
    if (ref_instance==NULL || ref_instance->uart_instance->uart_handle==NULL)
    {
        return;
    }

    if (ref_instance->uart_instance->uart_handle->ErrorCode & HAL_UART_ERROR_ORE) {
        __HAL_UART_CLEAR_OREFLAG(ref_instance->uart_instance->uart_handle); // 清除ORE错误标志
        // 重新启动DMA接收
        HAL_UART_Receive_DMA(ref_instance->uart_instance->uart_handle, ref_instance->uart_instance->rx_buff, 1);
    }

    if (ref_instance->uart_instance->uart_handle->ErrorCode & HAL_UART_ERROR_FE) {
        __HAL_UART_CLEAR_FEFLAG(ref_instance->uart_instance->uart_handle); // 清除ORE错误标志

        // 重新启动DMA接收
        HAL_UART_Receive_DMA(ref_instance->uart_instance->uart_handle, ref_instance->uart_instance->rx_buff, 1);
    }

    if (ref_instance->uart_instance->uart_handle->ErrorCode & HAL_UART_ERROR_NE) {
        __HAL_UART_CLEAR_NEFLAG(ref_instance->uart_instance->uart_handle); // 清除ORE错误标志

        // 重新启动DMA接收
        HAL_UART_Receive_DMA(ref_instance->uart_instance->uart_handle, ref_instance->uart_instance->rx_buff, 1);
    }
}



//环形缓冲区实现
void RefereeCircularBuffer_Init(RefereeCircularBuffer_t *cb) {
    cb->head = cb->tail = cb->count = 0;
}

bool RefereeCircularBuffer_Put(RefereeCircularBuffer_t *cb, const uint8_t *data, size_t len) {
    if (len > REF_CIR_BUFFER_SIZE - cb->count) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        cb->buffer[cb->head++] = data[i];
        if (cb->head >= REF_CIR_BUFFER_SIZE) {
            cb->head = 0;
        }
        cb->count++;
    }
    return true;
}

size_t RefereeCircularBuffer_Get(RefereeCircularBuffer_t *cb, uint8_t *out, size_t max_len) {
    size_t read = 0;
    while (read < max_len && cb->count > 0) {
        out[read++] = cb->buffer[cb->tail++];
        if (cb->tail >= REF_CIR_BUFFER_SIZE) {
            cb->tail = 0;
        }
        cb->count--;
    }
    return read;
}

bool RefereeCircularBuffer_IsEmpty(const RefereeCircularBuffer_t *cb) {
    return cb->count == 0;
}

size_t RefereeCircularBuffer_Available(const RefereeCircularBuffer_t *cb) {
    return cb->count;
}


