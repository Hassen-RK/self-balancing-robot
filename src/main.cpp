// ============================================================
//  main.cpp — Self-Balancing Robot
//  Phase 1, Step 2: Reading Raw MPU6050 Data
//
//  Now that we confirmed I2C works, we read actual sensor data.
//  We talk directly to MPU6050 registers — no library magic.
//  Understanding this is what separates junior from senior.
// ============================================================

#include <Arduino.h>
#include <Wire.h>
#include "config.h"

// ============================================================
//  MPU6050 Register Addresses
//  These come from the MPU6050 datasheet — every sensor has
//  a register map. You write to registers to configure,
//  you read from registers to get data.
// ============================================================
#define MPU_REG_PWR_MGMT_1   0x6B   // Power management — we wake it up here
#define MPU_REG_ACCEL_XOUT_H 0x3B   // First data register (accel X high byte)
#define MPU_REG_GYRO_XOUT_H  0x43   // First gyro register (gyro X high byte)
#define MPU_REG_CONFIG       0x1A   // DLPF config
#define MPU_REG_GYRO_CONFIG  0x1B   // Gyro full scale range
#define MPU_REG_ACCEL_CONFIG 0x1C   // Accel full scale range

// ============================================================
//  Scaling factors — convert raw int to real units
//  These come from the MPU6050 datasheet (sensitivity table)
//
//  Accel at ±2g range  → 16384 LSB per g
//  Gyro  at ±250°/s    → 131.0 LSB per °/s
// ============================================================
#define ACCEL_SCALE_FACTOR  16384.0f
#define GYRO_SCALE_FACTOR   131.0f

// ============================================================
//  Data structure — group related data together
//  This is clean embedded C++ practice
// ============================================================
struct ImuData {
    float accel_x, accel_y, accel_z;   // in g (1g = 9.81 m/s²)
    float gyro_x,  gyro_y,  gyro_z;    // in degrees per second
};

// ============================================================
//  Forward Declarations
// ============================================================
void    mpu6050_init();
ImuData mpu6050_read();
void    print_imu_data(const ImuData& data);
void    print_banner();

// ============================================================
//  setup()
// ============================================================
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(2000);

    print_banner();

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Serial.println("[INFO] I2C initialized");

    mpu6050_init();
    Serial.println("[INFO] MPU6050 ready");
    Serial.println("[INFO] Starting data stream...\n");
}

// ============================================================
//  loop()
// ============================================================
void loop() {
    ImuData data = mpu6050_read();
    print_imu_data(data);
    delay(IMU_SAMPLE_MS);   // 100Hz sample rate from config.h
}

// ============================================================
//  mpu6050_init()
//  Wake up the sensor and configure it.
//  By default MPU6050 starts in sleep mode — we must wake it.
// ============================================================
void mpu6050_init() {
    // Wake up — write 0 to PWR_MGMT_1 register
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU_REG_PWR_MGMT_1);
    Wire.write(0x00);   // 0 = wake up, clear sleep bit
    Wire.endTransmission();

    // Set accelerometer range to ±2g (most sensitive)
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU_REG_ACCEL_CONFIG);
    Wire.write(0x00);   // 0x00 = ±2g
    Wire.endTransmission();

    // Set gyroscope range to ±250°/s (most sensitive)
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU_REG_GYRO_CONFIG);
    Wire.write(0x00);   // 0x00 = ±250°/s
    Wire.endTransmission();

    // Set Digital Low Pass Filter to ~21Hz
    // This smooths out high-frequency noise at hardware level
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU_REG_CONFIG);
    Wire.write(0x04);   // DLPF setting 4 = 21Hz bandwidth
    Wire.endTransmission();

    Serial.println("[INFO] MPU6050 configured: ±2g | ±250°/s | DLPF 21Hz");
}

// ============================================================
//  mpu6050_read()
//  Reads all 6 axes from MPU6050 in one burst read.
//
//  Why burst read? The MPU6050 stores accel+gyro in 14
//  consecutive registers. Reading them all at once is faster
//  and guarantees they're from the same sample instant.
// ============================================================
ImuData mpu6050_read() {
    ImuData data;

    // Tell MPU6050 we want to start reading from ACCEL_XOUT_H
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU_REG_ACCEL_XOUT_H);
    Wire.endTransmission(false);    // false = don't release I2C bus yet

    // Request 14 bytes: 6 accel + 2 temp + 6 gyro
    Wire.requestFrom(MPU6050_ADDRESS, 14, true);

    // Each value is 16-bit split across 2 bytes (high + low)
    // Shift high byte left 8 bits, OR with low byte to combine
    int16_t raw_ax = (Wire.read() << 8) | Wire.read();
    int16_t raw_ay = (Wire.read() << 8) | Wire.read();
    int16_t raw_az = (Wire.read() << 8) | Wire.read();
    int16_t raw_tmp = (Wire.read() << 8) | Wire.read();  // temperature (unused)
    int16_t raw_gx = (Wire.read() << 8) | Wire.read();
    int16_t raw_gy = (Wire.read() << 8) | Wire.read();
    int16_t raw_gz = (Wire.read() << 8) | Wire.read();

    (void)raw_tmp;  // suppress unused variable warning

    // Convert raw values to real units using scale factors
    data.accel_x = raw_ax / ACCEL_SCALE_FACTOR;
    data.accel_y = raw_ay / ACCEL_SCALE_FACTOR;
    data.accel_z = raw_az / ACCEL_SCALE_FACTOR;
    data.gyro_x  = raw_gx / GYRO_SCALE_FACTOR;
    data.gyro_y  = raw_gy / GYRO_SCALE_FACTOR;
    data.gyro_z  = raw_gz / GYRO_SCALE_FACTOR;

    return data;
}

// ============================================================
//  print_imu_data()
// ============================================================
void print_imu_data(const ImuData& data) {
    // Accel in g
    Serial.printf("ACCEL | X: %6.3f  Y: %6.3f  Z: %6.3f  [g]\n",
                  data.accel_x, data.accel_y, data.accel_z);

    // Gyro in degrees/sec
    Serial.printf("GYRO  | X: %6.2f  Y: %6.2f  Z: %6.2f  [deg/s]\n",
                  data.gyro_x, data.gyro_y, data.gyro_z);

    Serial.println("---");
}

// ============================================================
//  print_banner()
// ============================================================
void print_banner() {
    Serial.println("====================================");
    Serial.println("   Self-Balancing Robot — v0.2.0   ");
    Serial.println("   Phase 1: IMU Raw Data Reading    ");
    Serial.println("====================================");
}
