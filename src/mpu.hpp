#ifndef MAGNETOMETER_HPP
#define MAGNETOMETER_HPP

#include <Arduino.h>
#include <MPU6050.h>

class AcceleroGyro
{
public:
  void begin() noexcept { mpu.initialize(); }

  bool testConnection() noexcept { return mpu.testConnection(); }

  void getMotion(float &ax, float &ay, float &az, float &gx, float &gy, float &gz)
  {
    int16_t accX, accY, accZ;
    int16_t gyrX, gyrY, gyrZ;

    mpu.getMotion6(&accX, &accY, &accZ, &gyrX, &gyrY, &gyrZ);

    ax = accX / 16384.0; // Convert to g
    ay = accY / 16384.0; // Convert to g
    az = accZ / 16384.0; // Convert to g

    gx = gyrX / 131.0; // Convert to degrees per second
    gy = gyrY / 131.0; // Convert to degrees per second
    gz = gyrZ / 131.0; // Convert to degrees per second
  }

private:
  MPU6050 mpu;
};

#endif