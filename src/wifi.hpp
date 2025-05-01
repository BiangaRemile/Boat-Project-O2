#include <WiFi.h>    // Include the WiFi library for ESP32
#include <Arduino.h> // Include the Arduino core library for basic functionality

IPAddress create_access_point(const char *ssid, const char *password)
{

    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1); // Same as local_ip
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP);  // Set the WiFi mode to Access Point

    if (!WiFi.softAPConfig(local_ip, gateway, subnet)) // Configure the Access Point with the given IP, gateway, and subnet
    {
        Serial.println("Failed to configure Access Point"); // Log failure to configure
        
        while(true) {}
    }

    WiFi.softAP(ssid, password); // Start the Access Point with the given SSID and password
    return WiFi.softAPIP();      // Return the IP address of the Access Point
}