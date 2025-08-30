//
// Created by DingYong on 2025/8/25.
//

#ifndef DRONE_BMI088_H
#define DRONE_BMI088_H

#include <stdint.h>

/* DateSheet 5.3.4
 * Accel_X_int16 = ACC_X_MSB * 256 + ACC_X_LSB
 * Accl_X_in_mg  = Accel_X_int16 / 32768 * 1000 * 2^(<0x41> + 1) * 1.5
 *
 * DateSheet 5.3.11
 * <0x41> 指代寄存器ACC_RANGE中的值
 * 0x00 -> 3g; 0x01 -> 6g; 0x02 -> 12g; 0x03 -> 24g;
 *
 * 注:ADC原始值 x 该系数 = xx(mg) -> (0.001 * 9.8m/s²)
 *    若希望得到以m/s²为单位
 *    请再 / 1000 * 9.8
*/

#define ACC_RANGE_3G_K    0.091552734375f
#define ACC_RANGE_6G_K    0.18310546875f
#define ACC_RANGE_12G_K   0.3662109375f
#define ACC_RANGE_24G_K   0.732421875f

/* DataSheet 1.3 Gyroscope Specifications
 *
 * 灵敏度系数Rfs 在手册的1.3章节
 *
 * 推导:
 * 满量程ADCfs = 2^15 - 1 = 32767LSB
 * 此刻,灵敏度系数Rfs(LSB/°/s) 的推导:
 * 灵敏度系数Rfs(LSB/°/s) = ADCfs(LSB) / range(°/s)
 *
 * Rfs125 -> 262.144 LBS/°/s; Rfs250 -> 131.072 LBS/°/s;
 * Rfs500 -> 65.536 LBS/°/s; Rfs1000 -> 32.768 LBS/°/s; Rfs2000 -> 16.384 LBS/°/s
 *
 * ADC原始值(LSB) = 角速度(°/s) x 灵敏度系数(LSB/°/s)
 * 可得:
 * 角速度(°/s) = ADC原始值(LSB) x 1/灵敏度系数(LSB/°/s)
 * 下将:
 * 1/灵敏度系数 宏定义:GYRO_RANGE_XX_K (°/s/LSB)
 * 本质:
 * 分辨率系数(°/s/LSB) = 范围(°/s) / ADC原始值(LSB)  ->  1LSB 对应 多少°/s 的变化量
 * -> 量程越大,1LSB 对应的 °/s 变化量越大 -> 分辨率越低
 *
 * 手册采用度每秒,此处系数采用弧度每秒,便于后续四元数运算
 */

#define GYRO_RANGE_125_K   0.000066579027251980956150958662738366f
#define GYRO_RANGE_250_K   0.00013315805450396191230191732547673f
#define GYRO_RANGE_500_K   0.00026631610900792382460383465095346f
#define GYRO_RANGE_1000_K  0.00053263221801584764920766930190693f
#define GYRO_RANGE_2000_K  0.00106526443603169529841533860381f

#pragma pack(1)
typedef struct {
    uint8_t status;
    int16_t accel[3];
    int16_t rate[3];
    int16_t temp;
} BMI088_RAW_DATA;

typedef struct BMI088_OUTPUT_DATA {
    uint8_t status;
    float accel[3];
    float rate[3];
    float temp;
    float time;
} BMI088_OUTPUT_DATA;

#pragma pack()

#endif //DRONE_BMI088_H