//
// Created by DingYong on 25-8-12.
//

#include "limit.h"

/**
 *
 * @param input
 * @param output_max
 * @param output_min
 * @return
 */
float setOutLimit(float const input,float const output_max,float const output_min) {
    if (input > output_max)           return output_max;
    else if (input < output_min)      return output_min;
    else                              return input;
}
