//
// Created by DingYong on 2025/8/25.
//

#include "bmi088.h"

#include <stdint.h>

#include "bmi088_register.h"

#define BMI088_ACCEL_K      ACC_RANGE_3G_K
#define BMI088_GYRO_K       GYRO_RANGE_2000_K

/*
 * 拉低BMI088_GYRO_CS_L(选中传感器)
 * 调用BMI088_write_single_reg((reg),(data)) (写寄存器)
 * 拉高BMI088_GYRO_CS_H(释放寄存器)
*/

#define BMI088_accel_write_single_reg(reg,data)   \
{                                                 \
    BMI088_GYRO_CS_L();                           \
    BMI088_write_single_reg((reg),(data));        \
    BMI088_GYRO_CS_H();                           \
}

#define BMI088_accel_read_single_reg(reg,data)    \
{                                                 \
    BMI088_ACCEL_CS_L();                          \
    BMI088_read_write
}
