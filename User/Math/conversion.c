//
// Created by DingYong on 25-8-11.
//

#include "conversion.h"

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
