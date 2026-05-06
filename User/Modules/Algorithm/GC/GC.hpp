//
// Created by DingYong on 2026/5/4.
//

#pragma once

#include "cmsis_os.h"
#include "math.h"
#include "pid.h"
#include "stdint.h"

#define DEFAULT_MIN_RAD    0.72f
#define DEFAULT_MAX_RAD    1.3f

#define RAD_TO_DEG         57.29578f
#define DEG_TO_RAD         0.0174533f
#define SUM_POINTS        200

#define DEFAULT_STEP_RAD   (1.0f * DEG_TO_RAD) // 步进角1度
#define DEFAULT_WAIT_SEC   3.0f  // 停顿3s,稳态后开始采样

class GravityCompensator {

public:
    float angles[SUM_POINTS];
    float torques[SUM_POINTS];
    uint16_t id = 0;
    float current_angle = DEFAULT_MIN_RAD;
    float a =0.0f, b = 0.0f, c = 0.0f;

    float  run(const pid_t * pid) {
        if (id < SUM_POINTS) {
            angles[id] = current_angle;
            torques[id] = pid->output;

            float out_angle = current_angle; // output for motor

            current_angle = DEFAULT_MIN_RAD + ((float)id + 1) * (DEFAULT_MAX_RAD - DEFAULT_MIN_RAD) / (SUM_POINTS - 1);
            id++;

            osDelay(5000);
            if (current_angle >= DEFAULT_MAX_RAD) {
                fit_quadratic();
                current_angle = DEFAULT_MAX_RAD;
            }

            return out_angle;
        } else {
            return current_angle;
        }
    }

    // feedback torque
    float feedback_torque(float angle) {
        return a*angle*angle + b*angle + c;
    }

private:
    void fit_quadratic() {

        int n = id;
        if (n < 3) return;

        // 二次拟合所需要的七个求和值
        float sumX=0, sumY=0, sumX2=0, sumX3=0, sumX4=0;
        float sumXY=0, sumX2Y=0;

        // 遍历所有的采样点
        for (int i=5; i<n-5; i++) {

            float x = angles[i];  // 角度:自变量
            float y = torques[i];  // 力矩:因变量

            // 提前算出x^2,x^3,x^4, 减少后续重复计算
            float x2 = x*x;
            float x3 = x2*x;
            float x4 = x3*x;

            sumX   += x;
            sumY   += y;
            sumX2  += x2;
            sumX3  += x3;
            sumX4  += x4;
            sumXY  += x*y;
            sumX2Y += x2*y;
        }


        // 构建 3x3 系数矩阵 M 和 常数矩阵 B
        // 二次拟合标准矩阵形式: M * [a; b; c] = B
        // 从而解出 a, b, c

        // 系数矩阵M
        float M00 = n,       M01 = sumX,    M02 = sumX2;  // 矩阵第一行
        float M10 = sumX,    M11 = sumX2,   M12 = sumX3;  // 矩阵第二行
        float M20 = sumX2,   M21 = sumX3,   M22 = sumX4;  // 矩阵第三行

        // 常数矩阵B
        float B0 = sumY;   // line 1
        float B1 = sumXY;  // line 2
        float B2 = sumX2Y; // line 3

        // 计算矩阵行列式
        // function: 1.判断矩阵是否可解 2.后续求逆矩阵
        float det = M00*(M11*M22 - M12*M21)
                  - M01*(M10*M22 - M12*M20)
                  + M02*(M10*M21 - M11*M20);

        // zero
        if (fabs(det) < 1e-6f) return;

        // 克莱姆法则求解 a, b, c
        // 方程组顺序: [c, b, a]^T
        c = ( B0 * (M11 * M22 - M12 * M21)
            - B1 * (M01 * M22 - M02 * M21)
            + B2 * (M01 * M21 - M11 * M02) ) / det;  // 常数项

        b = ( B0 * (M12 * M20 - M10 * M22)
            - B1 * (M02 * M20 - M00 * M22)
            + B2 * (M02 * M10 - M00 * M12) ) / det;  // 一次项系数

        a = -( B0 * (M11 * M20 - M10 * M21)
            - B1 * (M01 * M20 - M00 * M21)
            + B2 * (M01 * M10 - M00 * M11) ) / det;  // 二次项系数
    }
};