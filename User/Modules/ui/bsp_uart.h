/**
*   @file bsp_uart.h
*   @brief 串口通讯的实现
*   @author Wenxin HU
*   @date 25-7-11
*   @version 0.1
*   @note
*/
#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "usart.h"

#define UART_MAX_CNT 5 // 单个C板串口最大数量，如果使用其他开发版请自行配置
#define UART_RX_BUFF_LEN 50 // 串口缓存最大长度

/* 串口的通讯模式 */
typedef enum {
    UART_BLOCKING_MODE = 0, // 阻塞模式
    UART_IT_MODE = 1,       // 中断模式
    UART_DMA_MODE = 2,      //DMA模式
    UART_IDLE_MODE = 3,     //空闲中断模式
} UartMode_e;

/* UART实例化结构体 */
#pragma pack(1)
typedef struct _UartInstance_s {
    UART_HandleTypeDef* uart_handle;                          // 串口句柄
    uint8_t tx_buff[256];                                     // 发送缓存
    uint8_t rx_buff[256];                                     // 接收缓存
    uint16_t rx_len;                                          // 接收长度
    uint16_t tx_len;                                          //发送长度 小于256Byte
    UartMode_e mode;                                          // 串口通讯模式
    uint16_t rx_cnt;                                          // 通信计数，每次中断+1
    uint16_t tx_cnt;                                          // 通信计数，每次中断+1
    uint16_t rx_size;                                         // 不定长接收的接收到长度
    void (*uart_module_callback)(struct _UartInstance_s*);    // 接收的回调函数,用于解析接收到的数据
    void (*uart_module_tx_callback)(struct _UartInstance_s*); // 发送完成的回调函数
    void* id;                                                 //使用uart外设的模块指针(即id指向的模块拥有此uart实例,是父子关系)
} UartInstance_s;

/* UART实例初始化结构体,将此结构体指针传入注册函数 */
typedef struct {
    UART_HandleTypeDef* uart_handle;                  //串口句柄
    UartMode_e mode;                                  //串口通讯模式
    uint16_t rx_len;                                  //接收长度 小于256Byte
    void (*uart_module_callback)(UartInstance_s*);    // 接收的回调函数,用于解析接收到的数据
    void (*uart_module_tx_callback)(UartInstance_s*); // 发送完成的回调函数
    void* id;                                         //使用uart外设的模块指针(即id指向的模块拥有此uart实例,是父子关系)
} UartInitConfig_s;
#pragma pack()

/**
 * @file bsp_uart.h
 * @brief UART实例注册函数
 * @param config UART初始化配置结构体指针
 * @return instance指针--注册成功   NULL--注册失败
 * @date 2025-06-30
 */
UartInstance_s* Uart_Register(UartInitConfig_s* config);

/**
 * @file bsp_uart.h
 * @brief UART发送数据函数
 * @param uart_instance UART实例指针
 * @param data 发送数据指针
 * @return true--发送成功   false--发送失败
 * @date 2025-06-30
 */
bool Uart_Transmit(UartInstance_s* uart_instance, uint8_t* data);


/**
 * @file bsp_uart.h
 * @brief UART发送数据(需要指定长度)函数
 * @param uart_instance UART实例指针
 * @param data 发送数据指针
 * @param len 发送数据长度
 * @return true--发送成功   false--发送失败
 * @date 2025-12-14
 */
bool Uart_Transmit_Len(UartInstance_s* uart_instance, uint8_t* data, uint16_t len);

bool Uart_Blocking_Receive(UartInstance_s* uart_instance);

#endif //BSP_UART_H
