#pragma once
#include <Arduino.h>
#include <Wire.h>

/*
 * MPU6050 Driver
 *
 * I2C Communication Structure:
 *   1. beginTransmission(address) → open connection
 *   2. Wire.write(REGISTER)       → select register
 *   3. Wire.write(VALUE)          → write value
 *   4. endTransmission()          → close connection
 *
 * Power Management Register (0x6B):
 *   Default at power-on: 0x40 → SLEEP=1 (chip sleeping)
 *   We write 0x01 → SLEEP=0 (wake up) + CLKSEL=1 (gyro PLL clock)
 */

// ── Register Addresses ───────────────────────────────────────
#define MPU_ADDRESS          0x68  // I2C address (AD0=GND)
#define MPU_REG_PWR_MGMT     0x6B  // Register 107 — Power Management 1
#define MPU_REG_CONFIG       0x1A  // Register 26  — DLPF Configuration
#define MPU_REG_GYRO_CONFIG  0x1B  // Register 27  — Gyroscope Configuration
#define MPU_REG_GYRO_OUT     0x43  // Register 67  — Gyro output start (X high byte)

// ── Configuration Values ─────────────────────────────────────
#define MPU_DLPF_VALUE       0x05  // DLPF ~10Hz cutoff frequency
#define MPU_GYRO_FSR_VALUE   0x08  // gyro full scale range ±500°/s
#define GYRO_SCALE_FACTOR    65.5f // 65.5 LSB per °/s at ±500°/s

class mpu6050
{
private:
    float accel_x = 0.0F;     // accelerometer in g
    float accel_y = 0.0F;
    float accel_z = 0.0F;
    float gyro_x  = 0.0F;     // gyroscope in °/s (bias corrected)
    float gyro_y  = 0.0F;
    float gyro_z  = 0.0F;

    float rate_gyro_x = 0.0F; // gyro bias X — measured during calibration
    float rate_gyro_y = 0.0F; // gyro bias Y
    float rate_gyro_z = 0.0F; // gyro bias Z

    uint8_t _sda;              // SDA pin number
    uint8_t _scl;              // SCL pin number


public:
    mpu6050(uint8_t sda, uint8_t scl); // constructor — stores pins ONLY


    void begin();              // ← NEW — initializes hardware, call from setup()
    void mpu6050_gyro_read();  // reads gyro data

    // Getters — read-only access to private data
    float getGyroX() { return gyro_x; }
    float getGyroY() { return gyro_y; }
    float getGyroZ() { return gyro_z; }

    uint8_t getSDA() { return _sda; }
    uint8_t getSCL() { return _scl; }

    void calibration_gyro(uint16_t calib);

    ~mpu6050();                         // destructor  — intentionally empty
};