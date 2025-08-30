//
// Created by DingYong on 2025/8/25.
//

#include "bmi088.h"

#include <stdint.h>
#include "bmi088_register.h"
#include "spi_bmi088.h"

#define BMI088_ACCEL_K      ACC_RANGE_3G_K
#define BMI088_GYRO_K       GYRO_RANGE_2000_K

/*
 * 拉低BMI088_GYRO_CS_L(选中传感器)
 * 调用BMI088_write_single_reg((reg),(data)) (写寄存器)
 * 拉高BMI088_GYRO_CS_H(释放寄存器)
*/

#define BMI088_accel_write_single_reg(reg,data)    \
do {                                               \
BMI088_GYRO_CS_L();                                \
BMI088_write_single_reg((reg),(data));             \
BMI088_GYRO_CS_H();                                \
}while (0)

#define BMI088_accel_read_single_reg(reg,data)      \
do {                                                \
BMI088_ACCEL_CS_L();                                \
BMI088_Read_Write_Data((reg) | 0x80);               \
BMI088_Read_Write_Data(0x55);                       \
data = BMI088_Read_Write_Data(0x55);                \
}while (0)

#define BMI088_accel_write_muli_reg(reg, data, len) \
do {                                                \
BMI088_ACCEL_CS_L();                                \
BMI088_write_muli_reg(reg, data, len);              \
BMI088_ACCEL_CS_H();                                \
}while (0)


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

/**
 * @brief 从bmi088连续多个寄存器写入数据
 *        适配SPI突发读(burst-read operations):手册 6.1.2:起始地址最高位置1后,后续地址自动递增
 *        For example,to read the accelerometer values in SPI mode,the user has to
 *        read 7 bytes(加速度计读取时不会立即发送,而是先发送一字节无效数据,之后再传输有效数据,所以手册里面
 *        写的是7bytes,该spi特性只针对加速度计),starting from address 0x12(ACC data).
 *        From these bytes the user must discard the first byte and finds the
 *        acceleration information in byte #2-#7 (corresponding to the content of
 *        the addresses 0x12 - 0x17)
 * @param reg
 * @param buf
 * @param len
 */
void BMI088_write_muli_reg(const uint8_t reg, const uint8_t *buf, uint8_t len) {
    BMI088_Read_Write_Data(reg);
    while (len != 0) {
        BMI088_Read_Write_Data(*buf);
        buf++;
        len--;
    }
}

/**
 * @brief 从bmi088连续多个寄存器读取数据
 *        适配SPI突发读(burst-read operations):手册 6.1.2:起始地址最高位置1后,后续地址自动递增
 *        For example,to read the accelerometer values in SPI mode,the user has to
 *        read 7 bytes(加速度计读取时不会立即发送,而是先发送一字节无效数据,之后再传输有效数据,所以手册里面
 *        写的是7bytes,该spi特性只针对加速度计),starting from address 0x12(ACC data).
 *        From these bytes the user must discard the first byte and finds the
 *        acceleration information in byte #2-#7 (corresponding to the content of
 *        the addresses 0x12 - 0x17)
 * @param reg
 * @param buf
 * @param len
 */
void BMI088_read_muli_reg(uint8_t reg, uint8_t *buf, uint8_t len) {
    // 发送突发读 起始地址:DS 6.1.2,起始地址最高位置1(& 0x80)后,
    // 传感器会自动递增地址,无需重复发送后续地址
    BMI088_Read_Write_Data(reg | 0x80);

    while (len != 0) {
        // 逐字节读取数据:发送0x55触发数据返回
        // 0x55既不是寄存器地址,也不是特定命令
        // 发送0x55是为了满足SPI "发-收同步"的时序:SPI是全双工同步,主机每发1字节(不管发的是地址,0x55还是别的什么),同时收到1字节数据
        // 主机先发"带读命令的寄存器地址(reg | 0x80)" -> 对应时序里,SDI发送地址,SDO此时输出dummy字节,(传感器还没准备好数据)
        // 接着主机发0x55 -> 这一步是"强制触发一次SPI传输",让传感器有足够时钟周期,把真实读的寄存器值放到SDO上
        // simply put,0x55是"占位符",利用SPI全双工时序,逼着传感器将真实数据吐出来
        *buf = BMI088_Read_Write_Data(0x55);
        buf++;  // 缓冲区指针后移,存储下一字节数据
        len--;  // 剩余读取长度递减,直至完成所有字节读取
    }
}

void BMI088_sense_read(BMI088_OUTPUT_DATA *bmi088_output_data) {
    int16_t bmi088_raw_temp;
    BMI088_RAW_DATA bmi088_raw_data;

    BMI088_acc
}
