#include <compass.hpp>
#include <mpu.hpp>

DigitalCompass compass;
AcceleroGyro mpu;

void initializationHeading() {
    Wire.begin();
    compass.begin();
    mpu.begin();
}

float getHeading(unsigned long long &lastTime, float &pitch, float &roll, const float alpha) {
    float ax, ay, az; // Accéléromètre
    float gx, gy, gz; // Gyroscope
    float mx, my, mz; // Magnétomètre

    mpu.getMotion(ax, ay, az, gx, gy, gz);
    compass.getValues(mx, my, mz);

    unsigned long long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0; // Convert milliseconds to seconds
    lastTime = currentTime;

    float pitch_gyro = gy * dt * PI / 180;
    float roll_gyro = gx * dt * PI / 180;

    float pitch_accel = atan2(-ax, sqrt(ay * ay + az * az));
    float roll_accel = atan2(ay, az);

    pitch = alpha * (pitch + pitch_gyro) + (1 - alpha) * pitch_accel;
    roll = alpha * (roll + roll_gyro) + (1 - alpha) * roll_accel;

    float magX = mx * cos(pitch) + mz * sin(pitch);
    float magY = mx * sin(roll) * sin(pitch) + my * cos(roll) - mz * sin(roll) * cos(pitch);

    float heading = atan2(magY, magX) * 180 / PI;

    if (heading < 0) {
        heading += 360;
    }

    return heading;
}