//
// Created by DingYong on 2025/8/24.
//

#ifndef DRONE_SPI_BMI088_H
#define DRONE_SPI_BMI088_H

void BMI088_ACCEL_CS_L(void);
void BMI088_ACCEL_CS_H(void);
void BMI088_GYRO_CS_L(void);
void BMI088_GYRO_CS_H(void);
uint8_t BMI088_Read_Write_Data(uint8_t txData);

#endif //DRONE_SPI_BMI088_H