//
// Created by DingYong on 25-8-10.
//

#include "ist8310.h"

#include <stdint.h>

#include "main.h"
#include "stm32f4xx_hal.h"
#include "User/connectivity/iic/bsp_iic.h"

static const uint8_t ist8310_Write_Register_Data_Error[4][3] = {
    {0x0A, 0x0B, 0x01},     // 寄存器:0x0A   写入:0x0B|1011 (按手册的意思是保留值:200Hz输出)   错误码:0x01
    {0x0B, 0x08, 0x02},     // 寄存器:0x0B   写入:0x08|100  (开启中断并设置低电平)             错误码:0x02
    {0x41, 0x09, 0x03},     // 寄存器:0x41   写入:0x09|1001 (x和y和z都是平均采样2次)           错误码:0x03
    {0x42, 0xC0, 0x04}      // 寄存器:0x42   写入:0xC0|1100 0000 (默认优化设置)               错误码:0x04
};


uint8_t ist8310Init(void) {
    // 初始化GPIO和IIC通信
    ist8310GpioInit();
    ist8310ComInit();

    // 硬件复位: 拉低RST引脚, 延迟后再拉高
    ist8310_RST_L();
    HAL_Delay(50);
    ist8310_RST_H();
    HAL_Delay(50);

    // 读取设备ID寄存器WAI并验证传感器
    uint8_t data = ist8310ReadRegisterSingle(ist8310_WAI);
    if (data != ist8310_WAI_DefaultValue) {
        return IST8310_NO_SENSOR;           // 返回设备寻址失败
    }

    // 配置寄存器并验证
    for (uint8_t i = 0; i < 4; i++) {
        ist8310WriteRegisterSingle(ist8310_Write_Register_Data_Error[i][0],       // 向目标寄存器写入目标数据
                              ist8310_Write_Register_Data_Error[i][1]);
        HAL_Delay_us(150);                                                        // 等待150us写入完毕

        data = ist8310ReadRegisterSingle(ist8310_Write_Register_Data_Error[i][0]); // 检测寄存器中的真实数据是不是我刚刚写入的
        if (data != ist8310_Write_Register_Data_Error[i][1]) {                     // 如果不是
            return ist8310_Write_Register_Data_Error[i][2];                        // 返回对应写入过程的错误码
        }
    }

    return IST8310_OK;                                                             // 全部检测通过则返回IST8310_OK,表示初始化成功
}

void ist8310ReadData(float magneticField[3], float *temperature) {                    // 数组名函数形参时传入的实际是指向首元素的地址
    uint8_t rx_buffer_magn[6];
    uint8_t rx_buffer_temp[2];
    int16_t temporary_ist8310_data = 0;                               // 临时变量,用于暂存传感器输出的原始16bit数据(未转换成uT的磁场强度)

    ist8310ReadRegisterMultiple(0x03, rx_buffer_magn, 6);   // 将读到的6个寄存器数据存入接收缓冲区
                                                                      // (0x03 -> 0x08 | DATAXL -> DATAZH) (x轴第八位 -> z轴高八位)
    ist8310ReadRegisterMultiple(0x1C, rx_buffer_temp, 2);

    temporary_ist8310_data = ((uint16_t)rx_buffer_magn[1] << 8 | (uint16_t)rx_buffer_magn[0]);  // X轴:[-1600uT,1600uT]
    magneticField[0] = (float)temporary_ist8310_data * MAG_SEN;

    temporary_ist8310_data = (uint16_t)rx_buffer_magn[3] << 8 | (uint16_t)rx_buffer_magn[2];  // Y轴:[-1600uT,1600uT]
    magneticField[1] = (float)temporary_ist8310_data * MAG_SEN;

    temporary_ist8310_data = (uint16_t)rx_buffer_magn[5] << 8 | (uint16_t)rx_buffer_magn[4];  // Z轴:[-2500uT,2500uT]
    magneticField[2] = (float)temporary_ist8310_data * MAG_SEN;

    temporary_ist8310_data = (int16_t)rx_buffer_temp[1] << 8 | rx_buffer_temp[0];
    *temperature = (float)temporary_ist8310_data / 65535.0 * (85 + 40) - 40;      //85℃是最大温度,-40℃是最小温度
}

// 使用:
// 在USER CODE BEGIN4
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//     if (GPIO_Pin == IST8310_DRDY_Pin) {           // 需要根据芯片手册来配置外部终端输入,此处是 下降沿触发
//         ist8310ReadData(magneticField, &temperature);
//     }
//  }

// 由于iic依赖于系统时钟SystemClock_Config();
// 所以不推荐写在USER CODE BEGIN Init
// 而是在所有系统配置和HAL初始化完的USER CODE BEGIN 2
// ist8310Init()    (若初始化失败,可通过返回值来判断)

// 创建全局数组变量magneticField[3] [0]:X [1]:Y (X|Y 量程:+- 1600uT)  [2]:Z (Z 量程:+-2500uT)
// 创建温度: float temperature