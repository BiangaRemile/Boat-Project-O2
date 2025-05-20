#include <compass.hpp>
#include <mpu.hpp>
#include <pins.hpp>

// DigitalCompass compass;
AcceleroGyro *mpuHeading;
DigitalCompass *compass;
float accErrorX, accErrorY, gyrErrorX, gyrErrorY, gyrErrorZ;

void initializationHeading()
{
    Wire.begin(I2C_SDA, I2C_SCL); // Initialize I2C with custom SDA and SCL pins
    // compass.begin();
    mpuHeading = new AcceleroGyro(); // Create an instance of the AcceleroGyro class
    compass = new DigitalCompass();  // Create an instance of the DigitalCompass class
    mpuHeading->calculateError(accErrorX, accErrorY, gyrErrorX, gyrErrorY, gyrErrorZ, 200);
}

void pitchAndRoll(unsigned long long &lastTime, float &heading, const float alpha)
{
    float ax, ay, az, gx, gy, gz;
    float x_gauss = 0, y_gauss = 0, z_gauss = 0;
    float headingGet = 0;

    mpuHeading->getMotion(ax, ay, az, gx, gy, gz);
    compass->getValues(x_gauss, y_gauss, z_gauss);

    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    float accAngleX = (atan2(ay, sqrt(pow(ax, 2) + pow(az, 2)))) - (accErrorX * PI / 180);
    float accAngleY = (atan2(-ax, sqrt(pow(ay, 2) + pow(az, 2)))) - (accErrorY * PI / 180);

    gx -= gyrErrorX;
    gy -= gyrErrorY;
    gz += gyrErrorZ;

    float static gyroAngleX = 0;
    float static gyroAngleY = 0;
    float static yaw = 0;

    gyroAngleX = gyroAngleX + (gx * dt * PI / 180);
    gyroAngleY = gyroAngleY + (gy * dt * PI / 180);

    float roll = alpha * gyroAngleX + (1 - alpha) * accAngleX;
    float pitch = alpha * gyroAngleY + (1 - alpha) * accAngleY;
    yaw = yaw + gz * dt;

    float xh = x_gauss * cos(pitch) + z_gauss * sin(pitch);
    float yh = x_gauss * sin(roll) * sin(pitch) + y_gauss * cos(roll) - z_gauss * sin(roll) * cos(pitch);

    float calHeading = atan2(yh, xh) * 180 / PI;

    heading = (int) calHeading % 360;

    // roll = 0.8 * roll + 0.2 * rollCal;
    // pitch = 0.8 * pitch + 0.2 * pitchCal;
    // yaw = 0.8 * yaw + 0.2 * yawCal;
}
