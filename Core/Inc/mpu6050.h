//
// Created by wang on 2026/11/7.
//

#ifndef HOMEWORK1_MPU6050_H
#define HOMEWORK1_MPU6050_H

#include "i2c.h"

#define MPU6050_ADDR  0xD0

typedef struct {
    float ax, ay, az;   //单位 g
    float gx, gy, gz;   //单位°/s
}MPU6050_Data;

uint8_t MPU6050_Init(I2C_HandleTypeDef *hi2c);
void MPU6050_Read(I2C_HandleTypeDef *hi2c, MPU6050_Data *d);

#endif //HOMEWORK1_MPU6050_H
