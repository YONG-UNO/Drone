//
// Created by DingYong on 25-8-11.
//

#ifndef FLOAT_UINT_CONVERSION_H
#define FLOAT_UINT_CONVERSION_H
#include <stdint.h>

// 电机参数: 1圈 = 8192(目标范围), 原始编码1圈 = 2^14 = 16384
#define TARGE_RANGE      8191         // 目标范围: 0~8191(1圈CCW)
#define ORIGINAL_RANGE   16384        // 原始编码1圈的增量 (2^14 CCW)
#define ORIGINAL_OFFSET  32768        // 原始编码的偏移量 (2^15, 用于将负数转成正数)

uint16_t encoder_convert(int16_t raw_encoder);

float uint_to_float(int uint_val, float x_min_val, float x_max_val, int bit_width);
int float_to_uint(float float_val, float x_min_val, float x_max_val, int bit_width);

#endif //FLOAT_UINT_CONVERSION_H
