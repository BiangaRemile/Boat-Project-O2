#ifndef MAGNETOMETER_HPP
#define MAGNETOMETER_HPP

#include <Arduino.h>
#include <MPU6050.h>

class AcceleroGyro
{
public:

  AcceleroGyro() { mpu.initialize(); }

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

  void calculateError(float &accErrorX, float &accErrorY, float &gyrErrorX, float &gyrErrorY, float &gyrErrorZ, const int &samples = 200) {
    // Implement error calculation logic here if needed
    int c = 0;
    float ax, ay, az, gx, gy, gz;

    while (c < samples) {
      getMotion(ax, ay, az, gx, gy, gz);

      accErrorX += atan2(ay, sqrt(pow(ax, 2) + pow(az, 2))) * 180 / M_PI; // Convert to degrees
      accErrorY += atan2(ax, sqrt(pow(ay, 2) + pow(az, 2))) * 180 / M_PI; // Convert to degrees

      gyrErrorX += gx;
      gyrErrorY += gy;
      gyrErrorZ += gz;

      c++;
    }

    accErrorX /= samples;
    accErrorY /= samples;
    
    gyrErrorX /= samples;
    gyrErrorY /= samples; 
    gyrErrorZ /= samples;
  }

private:
  MPU6050 mpu;
};

#endif