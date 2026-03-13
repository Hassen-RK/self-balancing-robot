#include <Arduino.h>
#include "mpu6050.h"

// Global object — constructor runs here but ONLY stores pin numbers
// No hardware touched yet — safe!
mpu6050 imu(21, 22);
//          ↑   ↑
//         SDA  SCL

void setup() {
    Serial.begin(115200);
    
    imu.begin();
    // ↑ NOW hardware initializes — Arduino is fully ready at this point
    // Wire.begin(), setClock(), wake up, DLPF, gyro range all happen here
    delay(250);
    Serial.println("[INFO] MPU6050 ready");
}

void loop() {
    imu.mpu6050_gyro_read();  // read fresh data from sensor

    Serial.printf("Gyro | X: %6.2f  Y: %6.2f  Z: %6.2f  [deg/s]\n",
        imu.getGyroX(),
        imu.getGyroY(),
        imu.getGyroZ()
    );

    delay(10);  // 100Hz sample rate
}
