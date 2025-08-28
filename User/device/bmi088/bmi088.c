//
// Created by DingYong on 2025/8/25.
//

#include "bmi088.h"

#include <stdint.h>
#include "bmi088_register.h"
#include "spi_bmi088.h"

#define BMI088_ACCEL_K      ACC_RANGE_3G_K
#define BMI088_GYRO_K       GYRO_RANGE_2000_K

/**
 * @brief 向BMI088的 单个寄存器 写入 1字节数据
 *        根据手册 6.1.2
 *        Bit #0: 读写位:0 -> SDI; 1 -> SDO
 *        Bit #1-7: Address AD(6:0)
 *        Bit #8-15: When in write mode,these are the data SDI,which will be written into the address
 *                   When in read mode, these bits contain unpredictable(不可预测) values, and the user
 *                   has to read Bit #16-23 to get the actual data from the reading address.
 * @param reg 寄存器地址
 * @param data 寄存器写入数据
 */
void BMI088_write_single_reg(const uint8_t reg, const uint8_t data) {
    BMI088_Read_Write_Data(reg);
    BMI088_Read_Write_Data(data);
}

void BMI088_read_single_reg(const uint8_t reg, uint8_t *return_data) {
    BMI088_Read_Write_Data(reg | 0x80);
    *return_data = BMI088_Read_Write_Data(0x55);
}

/*
 * 拉低BMI088_GYRO_CS_L(选中传感器)
 * 调用BMI088_write_single_reg((reg),(data)) (写寄存器)
 * 拉高BMI088_GYRO_CS_H(释放寄存器)
*/

#define BMI088_accel_write_single_reg(reg,data)   \
do {                                              \
    BMI088_GYRO_CS_L();                           \
    BMI088_write_single_reg((reg),(data));        \
    BMI088_GYRO_CS_H();                           \
}while (0)

#define BMI088_accel_read_single_reg(reg,data)    \
do {                                              \
    BMI088_ACCEL_CS_L();                          \
    BMI088_Read_Write_Data((reg) | 0x80);         \
    BMI088_Read_Write_Data((reg) | 0x80);         \
                                                  \
}while (0)
