//
// Created by DingYong on 25-8-11.
//

#include "conversion.h"

#include <stdlib.h>
#include <tgmath.h>

/**
 * @brief 将原始编码器值转换为0~8192范围（CCW为正）
 * @param raw_encoder 原始编码器值（范围：-32768 ~ 32767）
 * @return 转换后的值（0~8192，CCW旋转递增）
 */
uint16_t encoder_convert(int16_t raw_encoder) {
    // 步骤1: 将原始值偏移为非负数 (-32768 -> 0, 32767 -> 65535)
    uint32_t offset_value = raw_encoder + ORIGINAL_OFFSET;

    // 步骤2: 按比例缩放到目标范围 (原始16384 -> 目标8192,即除以2)
    uint32_t scaled_value = offset_value / 2;

    // 步骤3: 取模去报值在0~8192范围内 (超过1圈后循环)
    uint16_t converted_value = scaled_value % TARGE_RANGE;

    return converted_value;
}

float convert(float raw) {
    raw /= 2;
    return (float)((int)raw % 8192);
}

/**
 * @brief 将浮点数拆分为整数部分和指定精度的小数部分
 * @use   mcu使用的是精简版c库,不支持浮点数格式化输出,将浮点数转化成两个整数格式化
 * @param x              输入的浮点数
 * @param integer_part   输出的整数部分(指针)
 * @param decimal_part   输出的小数部分(指针)
 * @param decimal_bit    保留的小数位数(如1表示保留1位小数)
 */
void float_to_intSplit(const float x, int *integer_part, int *decimal_part, uint8_t decimal_bit) {
    // 1.取绝对值,将x变为正数,方便后续操作
    const float abs_x = fabsf(x);

    // 2.取整,直接截断小数
    *integer_part = (int)x;
    // 3.恢复符号
    if (x < 0)   *integer_part = - (*integer_part);

    // 4.取出小数部分,并将小数部分放大为整数,四舍五入(加上0.5f,当超过1时自动截断实现四舍五入)
    int scale = 1;
    for (int i = 0; i < decimal_bit; i++) {
        scale *= 10;
    }
    *decimal_part = (int)( (abs_x - (float)(*integer_part)) * (float)scale + 0.5f);
}

/**
 * @brief  uint转float
 * @param  x_int   最大值
 * @param  x_min   最小值
 * @param  bits    位数
 */
float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

/**
 * @brief  float转uint
 * @param  x_int   最大值
 * @param  x_min   最小值
 * @param  bits    位数
 */
int float_to_uint(float x, float x_min, float x_max, int bits)
{
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}
