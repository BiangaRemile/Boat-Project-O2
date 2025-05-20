#ifndef GPS_HPP
#define GPS_HPP

#include <HardwareSerial.h>
#include <TinyGPS++.h>

class GPS
{

public:

    GPS(const int &rxPin, const int &txPin, const long &baudRate) : gpsSerial(1) // Use Serial1 for ESP32
    {
        gpsSerial.begin(baudRate, SERIAL_8N1, rxPin, txPin); // Initialize GPS serial communication
    }

    // Modified function: Waits for valid GPS data
    bool getGPSData(double &lat, double &lon, unsigned long timeout = 1000)
    {
        unsigned long start = millis();

        while (millis() - start < timeout)
        {
            while (gpsSerial.available() > 0)
            {
                if (gps.encode(gpsSerial.read()))
                {
                    if (gps.location.isValid())
                    {
                        lat = gps.location.lat();
                        lon = gps.location.lng();
                        return true; // Success
                    }
                }
            }
        }
        return false; // Timeout, no fix
    }
private:
    HardwareSerial gpsSerial; // Use HardwareSerial for ESP32
    TinyGPSPlus gps; // Use TinyGPSPlus library for parsing GPS data
};

#endif