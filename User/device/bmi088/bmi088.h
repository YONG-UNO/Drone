//
// Created by DingYong on 2025/8/25.
//

#ifndef DRONE_BMI088_H
#define DRONE_BMI088_H

/* DateSheet 5.3.4
 * Accel_X_int16 = ACC_X_MSB * 256 + ACC_X_LSB
 * Accl_X_in_mg  = Accel_X_int16 / 32768 * 1000 * 2^(<0x41> + 1) * 1.5
 *
 * DateSheet 5.3.11
 * <0x41> 指代寄存器ACC_RANGE中的值
 * 0x00 -> 3g; 0x01 -> 6g; 0x02 -> 12g; 0x03 -> 24g;
*/

#define ACC_RANGE_3g_k    0.091552734375f
#define ACC_RANGE_6g_k    0.18310546875f
#define ACC_RANGE_12g_k   0.3662109375f
#define ACC_RANGE_24g_k   0.732421875f


#endif //DRONE_BMI088_H