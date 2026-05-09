//
// Created by DingYong on 25-8-7.
//


#include "bsp_can.h"

#include "conversion.h"
#include "RS05.h"
#include "can.h"
#include "main.h"

// CAN1:fifo0: CAN_3508_M1_ID
//             CAN_3508_M2_ID
//             CAN_2006_M3_ID

// CAN2:fifo1: CAN_6020_M4_ID


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {

    //定义一个can消息头结构体变量,存储收到的can消息头部信息
    //包含消息id(StdId),帧类型(IDE,标准帧或拓展帧),消息类型(RTR,数据帧还是远程帧),数据长度(DLC,0-8字节)
    CAN_RxHeaderTypeDef rx_header;

    static uint8_t rx_data[8];                                                 //存储收到的CAN数据场(CAN最大支持8Byte数据)
    uint8_t n;                                                          //C语言标准规定:case后面不能直接声明变量

    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&rx_header,rx_data);   //hcan指针由回调函数触发后直接传入,rx_data[]是个语法糖,本质希望你传入指针

    switch (rx_header.StdId) {
        case CAN_3508_M1_ID:                                            //当收到的CAN消息ID(rx_header.StdId)
        case CAN_3508_M2_ID:                                            //与switch中的任意一个ID匹配时,就会执行{}中的代码
        case CAN_2006_M3_ID:
            n = rx_header.StdId - 0x201;                        //3508 & 2006: 0x200+n
            get_motor_measure(&motor_measure[n], rx_header.StdId ,rx_data);
            break;

        default:
            break;                                                      //如果一个都不匹配就直接退出
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {

    //定义一个can消息头结构体变量,存储收到的can消息头部信息
    //包含消息id(StdId),帧类型(IDE,标准帧或拓展帧),消息类型(RTR,数据帧还是远程帧),数据长度(DLC,0-8字节)
    CAN_RxHeaderTypeDef rx_header;

    static uint8_t rx_data[8];                                                 //存储收到的CAN数据场(CAN最大支持8Byte数据)
    uint8_t n;                                                          //C语言标准规定:case后面不能直接声明变量

    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1,&rx_header,rx_data);   //hcan指针由回调函数触发后直接传入,rx_data[]是个语法糖,本质希望你传入指针

    switch (rx_header.StdId) {
        case CAN_6020_M4_ID:
            n = rx_header.StdId - 0x202;                        // 6020: 0x204+1 - 0x202  = 3
            get_motor_measure(&motor_measure[n], rx_header.StdId, rx_data);
            break;

        case CAN_RS05_M6_ID:
            n = rx_header.StdId - 12;                          // RS05: 0x0C - 12 = 0
            get_motor_measure_RS05(&motor_measure_RS05[n], rx_header.StdId, rx_data);
            break;

        default:
            break;
        //如果一个都不匹配就直接退出
    }
}

void get_motor_measure(motor_measure_t *motor_measure,uint32_t StdId, uint8_t rx_data[]) {

    switch (StdId){
        case CAN_3508_M1_ID:
        case CAN_3508_M2_ID:
            motor_measure->last_ecd      =  motor_measure->ecd;
            motor_measure->ecd           =  ((uint16_t)rx_data[0] << 8  | (uint16_t)rx_data[1]);
            motor_measure->speed_rpm     =  ((int16_t) rx_data[2] << 8  | (int16_t)rx_data[3]);
            motor_measure->given_current =  ((int16_t) rx_data[4] << 8  | (int16_t)rx_data[5]);
            motor_measure->temperature   =  rx_data[6];
            motor_measure->torque        =  0;
            break;

        case CAN_2006_M3_ID:
            motor_measure->last_ecd      = motor_measure->ecd;
            motor_measure->ecd           = ((uint16_t)rx_data[0] << 8  | (uint16_t)rx_data[1]);
            motor_measure->speed_rpm     = ((int16_t) rx_data[2] << 8  | (int16_t)rx_data[3]);
            motor_measure->torque        = ((int16_t) rx_data[4] << 8  | (int16_t)rx_data[5]);
            motor_measure->given_current = 0;
            motor_measure->temperature   = 0;
            break;

        case CAN_6020_M4_ID:
            motor_measure->last_ecd      =  motor_measure->ecd;
            motor_measure->ecd           =  ((uint16_t)rx_data[0] << 8   | (uint16_t)rx_data[1]);
            motor_measure->speed_rpm     =  ((int16_t) rx_data[2] << 8  | (int16_t)rx_data[3]);
            motor_measure->given_current =  ((int16_t) rx_data[4] << 8  | (int16_t)rx_data[5]);
            motor_measure->temperature   =  rx_data[6];
            motor_measure->torque        =  0;
            break;

        default:
            motor_measure->ecd = 0;
            motor_measure->given_current = 0;
            motor_measure->last_ecd = 0;
            motor_measure->speed_rpm = 0;
            motor_measure->temperature= 0;
            motor_measure->torque = 0;
    }
}

void sendCmdShoot(int16_t frictionWheel_l, int16_t frictionWheel_r, int16_t dial) {
    uint32_t send_mail_box;
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = 0x200;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 0x08;

    uint8_t shoot_tx_message[8] = {0};
    shoot_tx_message[0] = (uint8_t)(frictionWheel_l >> 8);
    shoot_tx_message[1] = (uint8_t)frictionWheel_l;
    shoot_tx_message[2] = (uint8_t)(frictionWheel_r >> 8);
    shoot_tx_message[3] = (uint8_t)frictionWheel_r;
    shoot_tx_message[4] = (uint8_t)(dial >> 8);
    shoot_tx_message[5] = (uint8_t)dial;
    shoot_tx_message[6] = 0;
    shoot_tx_message[7] = 0;

    HAL_CAN_AddTxMessage(&hcan1,&tx_header,shoot_tx_message,&send_mail_box);
}

void sendCmdGimbal(int16_t yaw) {
    uint32_t send_mail_box0;
    uint32_t send_mail_box1;
    uint32_t send_mail_box2;

    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = 0X1FF;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 0x08;

    uint8_t yaw_tx_message[8] = {0};
    yaw_tx_message[0] = (uint8_t)(yaw >> 8);
    yaw_tx_message[1] = (uint8_t)yaw;
    yaw_tx_message[2] = 0;
    yaw_tx_message[3] = 0;
    yaw_tx_message[4] = 0;
    yaw_tx_message[5] = 0;
    yaw_tx_message[6] = 0;
    yaw_tx_message[7] = 0;

    if (HAL_CAN_AddTxMessage(&hcan2,&tx_header,yaw_tx_message,&send_mail_box0) != HAL_OK) {
        if (HAL_CAN_AddTxMessage(&hcan2,&tx_header,yaw_tx_message,&send_mail_box1) != HAL_OK) {
            HAL_CAN_AddTxMessage(&hcan2,&tx_header,yaw_tx_message,&send_mail_box2);
        }
    };
}

/**
 * @brief  BSP层CAN消息发送函数（兼容标准ID/扩展ID，带三重重试和空闲邮箱判断）
 * @param  hcan: CAN外设句柄指针，如 &hcan1、&hcan2
 * @param  can_id: CAN通信ID（32位无符号整数）
 *         - 标准ID（11bit）：取值范围 0 ~ 0x7FF
 *         - 扩展ID（29bit）：取值范围 0x800 ~ 0x1FFFFFFF
 * @param  tx_data: 待发送的8字节数据数组指针，不可为NULL
 * @retval bool 发送结果
 *         - true: 发送成功（帧已成功写入CAN空闲发送邮箱）
 *         - false: 发送失败（入参非法/无空闲邮箱/三次重试均失败）
 * @note   1. 函数固定发送8字节数据帧（DLC=0x08），不开启全局时间戳（避免覆盖tx_data[6][7]）
 * @note   2. 返回true仅表示帧进入邮箱，不代表接收设备（如电机）已成功接收
 * @note   3. 若需支持扩展ID，确保CAN外设已启用扩展帧功能（STM32 HAL库默认支持）
 */
bool BSP_CAN_Send_Msg(CAN_HandleTypeDef *hcan, uint32_t can_id, uint8_t tx_data[8]) {
    // 入参合法性检查
    if (hcan == NULL || tx_data == NULL) {
        return false;
    }

    uint32_t send_mail_box;
    CAN_TxHeaderTypeDef tx_header;

    // 通用CAN帧头配置
    tx_header.RTR   = CAN_RTR_DATA;           // 数据帧,一般不使用遥控帧
    tx_header.DLC   = 0x08;                   // 固定8字节数据长度
    tx_header.TransmitGlobalTime = DISABLE;   // 不开启时间戳,避免覆盖tx_data[6][7]

    // 兼容标准帧(11bit)和 拓展帧(29bit)
    if (can_id <= 0x7FF) {
        // 标准ID: 0~0x7FF, 配置为标准帧
        tx_header.IDE    = CAN_ID_STD;
        tx_header.StdId  = can_id;
    } else if (can_id <= 0x1FFFFFFF) {
        // 拓展ID: 0x800~0x1FFFFFFF, 配置为拓展帧
        tx_header.IDE    = CAN_ID_EXT;
        tx_header.ExtId  = can_id;
    }else {
        // 超出拓展ID范围,直接返回发送失败
        return false;
    }

    // 前置判断空闲邮箱，避免无意义重试
    if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        return false;  // 无空闲邮箱,直接返回失败
    }

    // 三重重试逻辑
    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &send_mail_box) == HAL_OK) {
        return true;
    }
    if (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &send_mail_box) == HAL_OK) {
        return true;
    }

    // 第三次发送：最后一次尝试，返回发送结果（成功/失败）
    return (HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data, &send_mail_box) == HAL_OK);
}

void CAN_Filter_Init(void)
{
    CAN_FilterTypeDef can1_filter_st;
    CAN_FilterTypeDef can2_filter_st;


    can1_filter_st.FilterIdHigh = 0x0000;
    can1_filter_st.FilterIdLow = 0x0000;
    can1_filter_st.FilterMaskIdHigh = 0x0000;
    can1_filter_st.FilterMaskIdLow = 0x0000;
    can1_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    can1_filter_st.FilterActivation = ENABLE;
    can1_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can1_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can1_filter_st.FilterBank = 0;
    can1_filter_st.SlaveStartFilterBank = 14;

    can2_filter_st.FilterIdHigh = 0x0000;
    can2_filter_st.FilterIdLow = 0x0000;
    can2_filter_st.FilterMaskIdHigh = 0x0000;
    can2_filter_st.FilterMaskIdLow = 0x0000;
    can2_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
    can2_filter_st.FilterActivation = ENABLE;
    can2_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can2_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can2_filter_st.FilterBank = 14;
    can2_filter_st.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &can1_filter_st) != HAL_OK)// 配置 CAN1 过滤器
    {
        Error_Handler();  // 处理错误
    }
    if (HAL_CAN_Start(&hcan1) != HAL_OK)// 启动 CAN1
    {
        Error_Handler();
    }
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)// 使能 CAN1 接收 FIFO0 消息中断
    {
        Error_Handler();
    }
    HAL_Delay(10);
    if (HAL_CAN_ConfigFilter(&hcan2, &can2_filter_st) != HAL_OK)    // 配置 CAN2 过滤器
    {
        Error_Handler();
    }
    if (HAL_CAN_Start(&hcan2) != HAL_OK)// 启动 CAN2
    {
        Error_Handler();
    }
    if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)// 使能 CAN2 接收 FIFO1 消息中断
    {
        Error_Handler();
    }
}
// //d.bus
// RC_t RC;
// uint8_t RC_Data[18] = {0};
//
// //can
// motor_measure_t motor_measure[8] = {0};
//
// // pid
// pid_t motor_2006_M3;
// pid_t angle_pid;
// pid_t speed_pid;
// /* USER CODE END PT
//
//
// pidInit(&angle_pid, 10, 0, 0, 1000, 500);
// pidInit(&speed_pid, 10, 0, 0, 1000, 500);
//
//
// CAN_Filter_Init();
//
// float input = 0;
//
//
// for (;;)
//   {
//     input = pidCascade(&angle_pid, &speed_pid,
//                 5000,
//                 motor_measure[2].ecd,
//                 motor_measure[2].speed_rpm,
//                 100);
//
//     if (RC.s2 == 1) {
//       // motor_2006_M3.output = RC.ch1;
//     sendCmdShoot(0,0,input);
//     }else if (RC.s2 == 3 || RC.s2 == 2) {
//       sendCmdShoot(0,0,0);
//     }
//
//     HAL_Delay(10);
//     /* USER CODE END WHILE */
//
// /* USER CODE BEGIN 3 */
//   }