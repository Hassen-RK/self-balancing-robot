#include "mpu6050.h"

// ONLY stores pin numbers — no hardware calls here!
// Hardware must not be touched before setup() runs
mpu6050::mpu6050(uint8_t sda, uint8_t scl)
{
    _sda = sda;  // store SDA pin
    _scl = scl;  // store SCL pin
    // nothing else here — Wire.begin() goes in begin()
}

// ── begin() ───────────────────────────────────────────────────
// Called from setup() — safe to touch hardware here
void mpu6050::begin()
{
    Wire.begin(_sda, _scl);      // start I2C with our pins
    Wire.setClock(400000);        // 400kHz fast mode AFTER begin()
    //            ↑
    //  setClock MUST come after begin() — not before!

    // Step 1: wake up + set stable clock source
    // Register 0x6B default = 0x40 (SLEEP=1)
    // Writing 0x01 = wake up + use gyro PLL clock
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_REG_PWR_MGMT);
    Wire.write(0x01);
    Wire.endTransmission();

    // Step 2: configure digital low pass filter
    // Register 0x1A — value 0x05 = ~10Hz cutoff
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_REG_CONFIG);
    Wire.write(MPU_DLPF_VALUE);
    Wire.endTransmission();

    // Step 3: set gyroscope full scale range to ±500°/s
    // Register 0x1B — value 0x08 = ±500°/s → 65.5 LSB/°/s
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_REG_GYRO_CONFIG);
    Wire.write(MPU_GYRO_FSR_VALUE);
    Wire.endTransmission();
}

// ── mpu6050_gyro_read() ───────────────────────────────────────
void mpu6050::mpu6050_gyro_read()
{
    // Step 1: point sensor to gyro start register
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(MPU_REG_GYRO_OUT);   // register 0x43 = gyro X high byte
    Wire.endTransmission();

    // Step 2: burst read 6 bytes (2 bytes × 3 axes)
    Wire.requestFrom(MPU_ADDRESS, 6);

    // Step 3: combine high + low bytes into signed 16-bit integer
    // int16_t MUST be signed — negative rotation = negative value
    int16_t raw_x = Wire.read() << 8 | Wire.read();  // X axis
    int16_t raw_y = Wire.read() << 8 | Wire.read();  // Y axis
    int16_t raw_z = Wire.read() << 8 | Wire.read();  // Z axis

    // Step 4: convert raw integer to °/s using scale factor
    gyro_x = (float)raw_x / GYRO_SCALE_FACTOR;
    gyro_y = (float)raw_y / GYRO_SCALE_FACTOR;
    gyro_z = (float)raw_z / GYRO_SCALE_FACTOR;
}

// ── Destructor ────────────────────────────────────────────────
mpu6050::~mpu6050()
{
    // intentionally empty — object lives until power off
}