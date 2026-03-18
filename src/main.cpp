#include <Arduino.h>
#include "mpu6050.h"

// global object — constructor stores pins only, safe here
mpu6050 imu(21, 22);

void setup() {
    Serial.begin(115200);
    delay(2000);   // wait for serial monitor to connect

    // Step 1: wake up sensor + configure registers
    imu.begin();
    // ↑ THIS was missing — without it sensor sleeps forever
    Serial.println("[OK] MPU6050 initialized");

    // Step 2: calibrate — robot must be completely still!
    Serial.println("[INFO] Calibrating... keep robot STILL for 2 seconds");
    imu.calibration_gyro(2000);   // 2000 samples × 1ms = 2 seconds
    Serial.println("[INFO] Calibration done — starting loop");
}

void loop() {
    imu.mpu6050_gyro_read();

    Serial.printf(">gyro_x:%.2f\n", imu.getGyroX());
    Serial.printf(">gyro_y:%.2f\n", imu.getGyroY());
    Serial.printf(">gyro_z:%.2f\n", imu.getGyroZ());

    delay(10U);   // 100Hz
}