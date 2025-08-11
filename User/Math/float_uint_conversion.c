//
// Created by DingYong on 25-8-11.
//

#include "float_uint_conversion.h"

/**
 * @brief 将无符号整数按比例映射到浮点数范围
 *
 * 线性转换函数：将指定位数的无符号整数转换为[x_min_val, x_max_val]区间内的浮点数
 * 映射关系：浮点数 = (整数 / 最大整数) * 区间跨度 + 最小值
 *
 * @param uint_val    输入的无符号整数（范围：0 至 2^bits - 1）
 * @param x_min_val   目标浮点数区间的最小值（转换下限）
 * @param x_max_val   目标浮点数区间的最大值（转换上限）
 * @param bit_width   无符号整数的位数（决定整数取值范围）
 * @return float      转换后的浮点数（落在[x_min_val, x_max_val]区间内）
 */
float uint_to_float(int uint_val, float x_min_val, float x_max_val, int bit_width) {
    float range_span = x_max_val - x_min_val;  // 浮点数区间的跨度
    float max_uint = (float)((1 << bit_width) - 1);  // 指定位数下的最大整数
    return ((float)uint_val) * range_span / max_uint + x_min_val;
}

/**
 * @brief 将浮点数按比例映射到无符号整数范围
 *
 * 线性转换函数：将[x_min_val, x_max_val]区间内的浮点数转换为指定位数的无符号整数
 * 映射关系：整数 = (浮点数 - 最小值) / 区间跨度 * 最大整数
 *
 * @param float_val   输入的浮点数（建议落在[x_min_val, x_max_val]区间内）
 * @param x_min_val   浮点数区间的最小值（转换基准下限）
 * @param x_max_val   浮点数区间的最大值（转换基准上限）
 * @param bit_width   目标无符号整数的位数（决定整数取值范围）
 * @return int        转换后的无符号整数（落在0至2^bits - 1区间内）
 */
int float_to_uint(float float_val, float x_min_val, float x_max_val, int bit_width) {
    float range_span = x_max_val - x_min_val;  // 浮点数区间的跨度
    float max_uint = (float)((1 << bit_width) - 1);  // 指定位数下的最大整数
    return (int)((float_val - x_min_val) * max_uint / range_span);
}
