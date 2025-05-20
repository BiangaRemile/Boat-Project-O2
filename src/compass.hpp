#ifndef COMPASS_HPP
#define COMPASS_HPP

#include <Arduino.h>         // Include Arduino core library for basic functionality
#include <QMC5883LCompass.h> // Include QMC5883L Compass library for magnetometer operations

/**
 * @brief A class to interface with the QMC5883L digital compass.
 *        Provides methods to initialize the compass and retrieve heading data.
 */
class DigitalCompass
{
public:
  DigitalCompass()
  {

    compass.init(); // reset sensor

    // Configurer le capteur en ±8 gauss
    Wire.beginTransmission(0x0D);
    Wire.write(0x09);
    Wire.write(0x1D); // 8G, ODR 200Hz, OSR 512, mode continu
    Wire.endTransmission();

  } // Constructor initializes the compass

  /**
   * @brief Reads the current heading from the compass and stores the X, Y, and Z axis values.
   *
   * @param x Reference to store the X-axis value (magnetic field strength).
   * @param y Reference to store the Y-axis value (magnetic field strength).
   * @param z Reference to store the Z-axis value (magnetic field strength).
   *
   * @details Calls compass.read() to fetch the latest sensor data and assigns the X, Y, and Z
   *          values to the provided references. Note: There is a bug in the original code where
   *          'y' is assigned twice instead of assigning 'z'. This should be corrected.
   */
  void getValues(float &x, float &y, float &z)
  {
    compass.read(); // Read the latest data from the compass

    x = compass.getX();
    y = compass.getY();
    z = compass.getZ();

    

    // azimuth = compass.getAzimuth(); // Get the azimuth value
  }

  /**
   * @brief Calculates the error in the compass readings over a specified number of samples.
   *
   * @param x Reference to store the calculated error for the X-axis.
   * @param y Reference to store the calculated error for the Y-axis.
   * @param z Reference to store the calculated error for the Z-axis.
   * @param samples Number of samples to average for error calculation (default is 200).
   *
   * @details This method collects multiple readings from the compass and averages them to
   *          determine the error in each axis. The results are stored in the provided references.
   *          The method can be used to calibrate the compass or to understand its accuracy.
   */

  void caculateError(float &headingError, const int &samples = 400)
  {
    // Implement error calculation logic here if needed
    int c = 0;
    float ax, ay, az, head;

    while (c < samples)
    {
      getValues(ax, ay, az);
      float head = atan2(ay, ax);
      headingError += (int) head; // Get the azimuth value
      c++;
    }

    headingError /= samples; // Average the error over the number of samples
  }

private:
  /**
   * @brief Instance of the QMC5883L compass driver.
   */
  QMC5883LCompass compass;
};

#endif