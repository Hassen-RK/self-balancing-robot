#pragma once

// --- I2C Pins ---
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22

// --- MPU6050 ---
#define MPU6050_ADDRESS     0x68
#define IMU_SAMPLE_RATE_HZ  100
#define IMU_SAMPLE_MS       (1000 / IMU_SAMPLE_RATE_HZ)

// --- Serial ---
#define SERIAL_BAUD_RATE    115200