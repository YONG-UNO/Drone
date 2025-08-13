//
// Created by DingYong on 25-8-7.
//


#include "bsp_can.h"

#include "can.h"
#include "main.h"
#include "../../Math/conversion.h"

// CAN1:fifo0: CAN_3508_M1_ID
//             CAN_3508_M2_ID
//             CAN_2006_M3_ID

// CAN2:fifo1: CAN_6020_M4_ID
//             CAN_4310_M5_ID


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {

    //定义一个can消息头结构体变量,存储收到的can消息头部信息
    //包含消息id(StdId),帧类型(IDE,标准帧或拓展帧),消息类型(RTR,数据帧还是远程帧),数据长度(DLC,0-8字节)
    CAN_RxHeaderTypeDef rx_header;

    uint8_t rx_data[8];                                                 //存储收到的CAN数据场(CAN最大支持8Byte数据)
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

    uint8_t rx_data[8];                                                 //存储收到的CAN数据场(CAN最大支持8Byte数据)
    uint8_t n;                                                          //C语言标准规定:case后面不能直接声明变量

    HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO1,&rx_header,rx_data);   //hcan指针由回调函数触发后直接传入,rx_data[]是个语法糖,本质希望你传入指针

    switch (rx_header.StdId) {
        case CAN_6020_M4_ID:
            n = rx_header.StdId - 0x202;                        // 6020: 0x204+1 - 0x202  = 3
            get_motor_measure(&motor_measure[n], rx_header.StdId, rx_data);
            break;

        case CAN_4310_M5_ID:
            n = rx_header.StdId - 17;                         // 4310: 0x11 - 17 = 0
            get_motor_measure_DM4310(&motor_measure_DM4310[n], rx_header.StdId, rx_data);
            break;

        default:
            break;                                              //如果一个都不匹配就直接退出
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
            motor_measure->ecd           =  ((int16_t)rx_data[0] << 8   | (int16_t)rx_data[1]);
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

void get_motor_measure_DM4310(motor_measure_DM4310_t *motor_measure_DM4310,uint32_t StdId, uint8_t rx_data[]) {
    motor_measure_DM4310->p_int = (rx_data[1] << 8) | rx_data[2];
    motor_measure_DM4310->v_int = (rx_data[3] << 4) | (rx_data[4] >> 4);
    motor_measure_DM4310->t_int = ((rx_data[4] & 0xF) << 8) | rx_data[5];
    motor_measure_DM4310->position = convert(motor_measure_DM4310->p_int);
    motor_measure_DM4310->velocity = uint_to_float(motor_measure_DM4310 -> v_int, -VMAX, VMAX, 12);
    motor_measure_DM4310->torque   = uint_to_float(motor_measure_DM4310 -> t_int, -TMAX, TMAX, 12);
}

void sendCmdShoot(int16_t frictionWheel_l, int16_t frictionWheel_r, int16_t dial) {
    uint32_t send_mail_box;
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = 0x200;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 0x08;

    uint8_t shoot_tx_message[8] = {0};
    shoot_tx_message[0] = frictionWheel_l >> 8;
    shoot_tx_message[1] = frictionWheel_l;
    shoot_tx_message[2] = frictionWheel_r >> 8;
    shoot_tx_message[3] = frictionWheel_r;
    shoot_tx_message[4] = dial >> 8;
    shoot_tx_message[5] = dial;
    shoot_tx_message[6] = 0;
    shoot_tx_message[7] = 0;

    HAL_CAN_AddTxMessage(&hcan1,&tx_header,shoot_tx_message,&send_mail_box);
}

void sendCmdGimbal_DM4310(float torq) {
    uint32_t send_mail_box;
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = 0x01;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 0x08;

    float    pos = 0,vel = 0,kp = 0,kd = 0;
    uint16_t pos_tmp,vel_tmp,kp_tmp,kd_tmp,tor_tmp;
    pos_tmp = float_to_uint(pos, -PMAX, PMAX, 16);
    vel_tmp = float_to_uint(vel, -VMAX, VMAX, 12);
    kp_tmp = float_to_uint(kp, 0,0,12);
    kd_tmp = float_to_uint(kd, 0, 0, 12);
    tor_tmp = float_to_uint(torq, -TMAX, TMAX, 12);

    uint8_t gimbal_tx_message[8] = {0};
    gimbal_tx_message[0] = (pos_tmp >> 8);
    gimbal_tx_message[1] = pos_tmp;
    gimbal_tx_message[2] = (vel_tmp >> 4);
    gimbal_tx_message[3] = ((vel_tmp&0xF)<<4)|(kp_tmp>>8);
    gimbal_tx_message[4] = kp_tmp;
    gimbal_tx_message[5] = (kd_tmp >> 4);
    gimbal_tx_message[6] = ((kd_tmp&0xF)<<4)|(tor_tmp>>8);
    gimbal_tx_message[7] = tor_tmp;

    HAL_CAN_AddTxMessage(&hcan2,&tx_header,gimbal_tx_message,&send_mail_box);
}

// DM4310使能
uint8_t DM4310_Enable_Array[8]  = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};		  // 电机使能命令
uint8_t DM4310_Disable_Array[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD};       // 电机失能命令

void DM4310_Enable(void) {
    uint32_t send_mail_box;
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = 0x01;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 0x08;

    if (HAL_CAN_AddTxMessage(&hcan2,&tx_header,DM4310_Enable_Array,&send_mail_box) != HAL_OK) {
        if (HAL_CAN_AddTxMessage(&hcan2, &tx_header, DM4310_Enable_Array, &send_mail_box) != HAL_OK) {
            HAL_CAN_AddTxMessage(&hcan2, &tx_header, DM4310_Enable_Array, &send_mail_box);
        }
    }
}

void DM4310_Disable(void) {
    uint32_t send_mail_box;
    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = 0x01;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 0x08;

    if (HAL_CAN_AddTxMessage(&hcan2,&tx_header,DM4310_Disable_Array,&send_mail_box) != HAL_OK) {
        if (HAL_CAN_AddTxMessage(&hcan2, &tx_header, DM4310_Disable_Array, &send_mail_box) != HAL_OK) {
            HAL_CAN_AddTxMessage(&hcan2, &tx_header, DM4310_Disable_Array, &send_mail_box);
        }
    }
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