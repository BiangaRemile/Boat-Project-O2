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
  void begin() { compass.init(); };

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
    compass.read();     // Read the latest data from the compass
    x = compass.getX(); // Get the X-axis value
    y = compass.getY(); // Get the Y-axis value
    z = compass.getZ(); // Get the Z-axis value (FIXED: Original code incorrectly assigned 'y' twice)
  }

private:
  /**
   * @brief Instance of the QMC5883L compass driver.
   */
  QMC5883LCompass compass;
};

#endif