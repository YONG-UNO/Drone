/**
 * @brief BMI088寄存器宏定义,沿用BMI088_DataSheet定义名称
 * @brief 内容引用BMI088_DataSheet V1.9
 */

#ifndef BMI088_REGISTER_H
#define BMI088_REGISTER_H

/*
 * 5.3.1 Register 0x00:ACC_CHIP_ID
 * bit:[7:0] readOnly ResetValue: 0x1E Contains identifier code of acceleration sensor
 */
#define ACC_CHIP_ID        0x00    // the register is "Who am i"
#define ACC_CHIP_ID_VALUE  0x1E

/*
 * 加速度计X,Y,Z轴的原始数据寄存器
5.3.4 Register 0x12 – 0x17: ACC data
Registers containing the acceleration sensor output. The sensor output is stored as signed 16 bit number
in 2’s complement format in each 2 registers. From the registers, the acceleration values can be
calculated as follows:
Accel_X_int16 = ACC_X_MSB * 256 + ACC_X_LSB
Accel_Y_int16 = ACC_Y_MSB * 256 + ACC_Y_LSB
Accel_Z_int16 = ACC_Z_MSB * 256 + ACC_Z_LSB
When a register is read containing the LSB value of an acceleration value, the corresponding MSB
register is locked internally, until it is read. By this mechanism, it is ensured that both LSB and MSB
values belong to the same acceleration value and are not updated between the readouts of the individual
registers.
The unit is in LSB. The conversion from LSB to acceleration (mg) is based on the range settings and
can be calculated as follows (<0x41>: content of the ACC_RANGE register):
Accel_X_in_mg = Accel_X_int16 / 32768 * 1000 * 2^(<0x41> + 1) * 1.5
Accel_Y_in_mg = Accel_Y_int16 / 32768 * 1000 * 2^(<0x41> + 1) * 1.5
Accel_Z_in_mg = Accel_Z_int16 / 32768 * 1000 * 2^(<0x41> + 1) * 1.5
*/

#define ACC_X_LSB          0x12
#define ACC_X_MSB          0x13
#define ACC_Y_LSB          0x14
#define ACC_Y_MSB          0x15
#define ACC_Z_LSB          0x16
#define ACC_Z_MSB          0x17

/*
 * 时间戳数据的底/中/高字节寄存器地址
*5.3.5 Register 0x18 – 0x1A: Sensortime data
Registers containing the value of the internal 24-bit counter.
包含24位内部寄存器的值(可以用秒,分,时理解)
• Register 0x18 (SENSORTIME_0) contains the lower 8 bits of the counter. This register is
incremented every 39.0625 μs.
0x18溢出周期:低八位范围0x00~0xFF,共256个值,每39.0625us加1 - > 溢出周期: 256 * 39.0625us = 10000us = 10ms

• Register 0x19 (SENSORTIME_1) contains the middle 8 bits of the counter. This register is
incremented on SENSORTIME_0 overflow, which is every 10 ms.
0x19溢出周期:低八位范围0x00~0xFF,共256个值,每10ms加1 - > 溢出周期: 256 * 10ms = 2560ms = 2.56s

• Register 0x1A (SENSORTIME_2) contains the higher 8 bits of the counter. This register is
incremented on SENSORTIME_1 overflow, which is every 2.56 s.
0x1A溢出周期:低八位范围0x00~0xFF,共256个值,每2.56s加1 - > 溢出周期: 256 * 2.56s = 65536ms = 655.36s

The complete 24-bit counter overflows after 655.36 s or almost 11 minute
*/
#define SENSORTIME_0       0x18
#define SENSORTIME_1       0x19
#define SENSORTIME_2       0x20

/*
*5.3.7 Register 0x22 – 0x23: Temperature sensor data
Registers containing the temperature sensor data output. The data is stored in an 11-bit value in 2’s
complement format. The resolution is 0.125°C/LSB, thus the temperature can be obtained as follows:
Temp_uint11 = (TEMP_MSB * 8) + (TEMP_LSB / 32)
if Temp_uint11 > 1023:
Temp_int11 = Temp_uint11 – 2048
else:
Temp_int11 = Temp_uint11
Temperature = Temp_int11 * 0,125°C/LSB + 23°C

The temperature sensor data is updated every 1.28 s.
*/

#define TEMP_MSB   0x22
#define TEMP_LSB   0x23

/*
 *5.3.10 Register 0x40:ACC_CONF
 Accelerometer configuration register
 */
#define ACC_ODR_OFFSET     0x00
#define ACC_ODR_12_5_HZ    (0x05 << ACC_ODR_OFFSET)
#define ACC_ODR_25_HZ      (0x06 << ACC_ODR_OFFSET)
#define ACC_ODR_50_HZ      (0x07 << ACC_ODR_OFFSET)
#define ACC_ODR_100_HZ     (0x08 << ACC_ODR_OFFSET)
#define ACC_ODR_200_HZ     (0x09 << ACC_ODR_OFFSET)
#define ACC_ODR_400_HZ     (0x0A << ACC_ODR_OFFSET)
#define ACC_ODR_800_HZ     (0x0B << ACC_ODR_OFFSET)
#define ACC_ODR_1600_HZ    (0x0C << ACC_ODR_OFFSET)

/*
 * 5.3.11 Register 0x41:ACC_RANGE
 Accelerometer range setting register
 */
#define ACC_RANGE          0x41

#define ACC_RANGE_OFFSET   0x00
#define ACC_RANGE_3g       (0x00 << ACC_RANGE_OFFSET)
#define ACC_RANGE_6g       (0x01 << ACC_RANGE_OFFSET)
#define ACC_RANGE_12g      (0x02 << ACC_RANGE_OFFSET)
#define ACC_RANGE_24g      (0x03 << ACC_RANGE_OFFSET)

/*
 * 采用RATE命名而不是GYRO,是因为这里表示角速度angle rate 而非 陀螺仪gyro传感器名称
* 5.5.2 Register 0x02 – 0x07: Rate data
Registers containing the angular velocity sensor output. The sensor output is stored as signed 16-bit
number in 2’s complement format in each 2 registers. From the registers, the gyro values can be
calculated as follows:
Rate_X: RATE_X_MSB * 256 + RATE_X_LSB
Rate_Y: RATE_Y_MSB * 256 + RATE_Y_LSB
Rate_Z: RATE_Z_MSB * 256 + RATE_Z_LSB
When a register is read containing the LSB value of a rate value, the corresponding MSB register is
locked internally, until it is read. By this mechanism, it is ensured that both LSB and MSB values belong
to the same rate range value and are not updated between the readouts of the individual registers.
The unit is in LSB. The conversion from LSB to angular velocity (degree per second) is based on the
range settings (see 5.5.5). For example, for the default range setting of 0x00 in register 0x0F, the
following conversion table applies:
Sensor output [LSB] Angular rate (in 2000°/s range mode)
+32767 + 2000°/s
… …
0 0°/s
… …
-32767 - 2000°/s
*/

#define RATE_X_LSB         0x02
#define RATE_X_MSB         0x03
#define RATE_Y_LSB         0x04
#define RATE_Y_MSB         0x05
#define RATE_Z_LSB         0x06
#define RATE_Z_MSB         0x07

/*
 * GYRO_RANGE_SETTING
 * Each range is from negative to positive
*/

#define GYRO_RANGE         0x0F

#define GYRO_RANGE_OFFSET  0x00
#define GYRO_RANGE_2000    (0x00 << GYRO_RANGE_OFFSET)
#define GYRO_RANGE_1000    (0x01 << GYRO_RANGE_OFFSET)
#define GYRO_RANGE_500     (0x02 << GYRO_RANGE_OFFSET)
#define GYRO_RANGE_250     (0x03 << GYRO_RANGE_OFFSET)
#define GYRO_RANGE_125     (0x04 << GYRO_RANGE_OFFSET)

/*
 * 设置陀螺仪的 输出数据速率(ODR,Output Data Rate) 与 低通滤波器(LPF,Low-pass Fliter) 带宽(BW,bandwidth)
 * 两者通过预设对应关系绑定,无需单独配置
 *
 * 两个关键规律:
 * 1.相同ODR可匹配不同带宽: 如ODR = 2000Hz时,可通过gyro_bw = 0x00(带宽532Hz) 或 gyro_bw = 0x01(带宽230Hz) 选择不同滤波强度
 * 2.带宽随ODR降低而减小: 如ODR从 2000Hz 降至 100Hz时, 默认匹配的带宽从 532Hz 逐步降至 12Hz, 符合"低ODR场景需要更强滤波以降低噪声"的硬件设计逻辑
 *
 * 实际配置:
 *  Note: bit #7 is read-only and always ‚1‘, but has no function and can safely be ignored.
 *  文档明确标准:"bit#7 为只读位, 始终为'1', 无实际功能, 可安全忽略"
 *
 *      1.配置值写入逻辑: 由于bit7为只读且固定为 1 , 写入配置时需确保"低七位为目标值,bit7无需手动设置,给 0 即可"
 *                     例如,需配置ODR = 100Hz,带宽 32Hz(0x07) ,则写入寄存器(0x01)的值可以是0x07(bit7 = 0, bit6~bit0 = 0x07)
 *      2.与应用场景的匹配建议:1.高动态场景(如无人机姿态控制):需要较高ODR与较高带宽,可选择(ODR = 2000Hz,带宽532Hz),确保快速响应旋转运动
 *                         2.低噪声场景(如静态角度测量):需要低ODR与低带宽,可选择(ODR = 100Hz,带宽 12Hz),通过强滤波抑制环境噪声
 *      3.配置后,陀螺仪会立即按照新的ODR和LPF输出数据,无需复位操作
 *
 *对不同场景的进一步解释:
 *1.高动态场景(无人机姿态控制):0x00(ODR = 2000Hz, BW = 532Hz)更合适
 *   1.高ODR(2000Hz)意味着陀螺仪每秒输出 2000 组角速度数据,能快速捕捉无人机高速旋转(如急转弯,姿态调整)
 *     时的动态变化,避免因数据更新来不及导致的控制延迟
 *   2.较高带宽(532Hz) 可确保高频旋转信号(如无人机螺旋转振动叠加的姿态变化) 不被过度滤波,完整保留动态运动细节
 *     满足高动态场景对"响应速度" 和 "信号完整性" 的双重需求
 *
 *2.低动态场景(静态角度测量):0x05(ODR = 100Hz, BW = 12Hz)更合适
 *   1.低ODR(100Hz) 在静态场景下可减少数据冗余,同时降低传感器自身的功耗与热噪声
 *   2.低带宽(12Hz) 能有效滤除环境中的高频噪声 (如空气扰动,机械微小振动),使角速度数据更稳定,
 *     避免噪声干扰导致的静态角度计算误差,低ODR+低带宽,可以应对低噪声,高精度静态测量场景
*/
#define GYRO_BANDWIDTH     0x10

#define GYRO_ODR_2000_BW_532    0x00
#define GYRO_ODR_2000_BW_230    0x01
#define GYRO_ODR_1000_BW_116    0x02
#define GYRO_ODR_400_BW_47      0x03
#define GYRO_ODR_200_BW_23      0x04
#define GYRO_ODR_100_BW_12      0x05
#define GYRO_ODR_200_BW_64      0x06
#define GYRO_ODR_100_BW_32      0x07

#endif
