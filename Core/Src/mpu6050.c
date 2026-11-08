#include "mpu6050.h"


uint8_t MPU6050_Init(I2C_HandleTypeDef *hi2c)
{   uint8_t id, data;
    if (HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, 0x75, I2C_MEMADD_SIZE_8BIT, &id, 1, 100) != HAL_OK)
        return 1;

    if (id != 0x74)
        return 2;

    data = 0x80;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x6B, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    HAL_Delay(100);
    data = 0x01;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x6B, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    data = 0x00;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x6C, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    data = 0x04;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x19, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    data = 0x03;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1A, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    data = 0x08;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1B, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    data = 0x00;
    HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, 0x1C, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    HAL_Delay(50);
    return 0;
}

void MPU6050_Read(I2C_HandleTypeDef *hi2c, MPU6050_Data *d) {
    uint8_t b[14];
    if (HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, 0x3B, I2C_MEMADD_SIZE_8BIT, b, 14, 100) != HAL_OK) {
        return;
    }
    int16_t ax = (int16_t)((b[0] << 8) | b[1]);
    int16_t ay = (int16_t)((b[2] << 8) | b[3]);
    int16_t az = (int16_t)((b[4] << 8) | b[5]);
    int16_t gx = (int16_t)((b[8] << 8) | b[9]);
    int16_t gy = (int16_t)((b[10] << 8) | b[11]);
    int16_t gz = (int16_t)((b[12] << 8) | b[13]);

    d->ax = ax / 16384.0f;
    d->ay = ay / 16384.0f;
    d->az = az / 16384.0f;
    d->gx = gx / 65.5f;
    d->gy = gy / 65.5f;
    d->gz = gz / 65.5f;

}
