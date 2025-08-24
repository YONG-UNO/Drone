//
// Created by DingYong on 2025/8/24.
// @brif 用于bmi088 spi bsp层封装

#include "main.h"
#include "stm32f4xx_hal_gpio.h"

#include "spi_bmi088.h"

#include "spi.h"

// SPI通讯时必须先拉低对应传感器对应的片选,通讯完成后拉高,避免总线上出现多个从设备同时响应

// 加速度计片选拉低(选中加速度计,准备spi通讯)
void BMI088_ACCEL_CS_L(void) {
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
}

// 加速度计片选拉高(释放加速度计,结束spi通讯)
void BMI088_ACCEL_CS_H(void) {
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
}

// 陀螺仪片选拉低(选中陀螺仪,准备spi通讯)
void BMI088_GYRO_CS_L(void) {
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
}

// 陀螺仪片选拉高(释放陀螺仪,结束spi通讯)
void BMI088_GYRO_CS_H(void) {
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);
}

uint8_t BMI088_Read_Write_Data(uint8_t txData) {
    uint8_t rxData;
    HAL_SPI_TransmitReceive_DMA(&hspi1, &txData, &rxData, sizeof(txData));
}