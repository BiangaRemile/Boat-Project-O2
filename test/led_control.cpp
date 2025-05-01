#include <Arduino.h>           // Include the Arduino core library
#include <webserver.hpp>      // Include ArduinoJson for JSON parsing
#include <WiFi.h>              // Include WiFi library to connect to a network
#include "SPIFFS.h"            // Include SPIFFS (file system) for serving files
#include "pins.hpp"            // Include custom pin definitions 

// WiFi credentials (replace with your own SSID and password)
const char *ssid = "***"; // WiFi SSID
const char *password = "Mama@2025"; // WiFi password


/**
 * @brief Initializes the ESP32 and sets up the web server and WebSocket.
 */
void setup()
{
    pinMode(WIFI_LED, OUTPUT); // Set the WiFi LED pin as output

    Serial.begin(9600); // Initialize serial communication at 9600 baud rate

    WiFi.mode(WIFI_STA);        // Set the WiFi mode to station (client)
    WiFi.begin(ssid, password); // Connect to the specified WiFi network

    byte result = WiFi.waitForConnectResult(); // Wait for the connection result

    if (result != WL_CONNECTED) // If the connection fails
    {
        Serial.println("Socket connection failed!"); // Log the failure
        return;
    }
    else // If the connection succeeds
    {
        Serial.println("Socket connected successfully!"); // Log the success
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP()); // Print the local IP address
    }

    WebServerManager server(80, "/ws");

    while (true) // Infinite loop to periodically send data
    {
        static unsigned long lastSendTime = 0; // Track the last send time
        unsigned long currentTime = millis();  // Get the current time in milliseconds
        if (currentTime - lastSendTime >= 500) // If 0.5 seconds have passed
        {
            lastSendTime = currentTime;             // Update the last send time
            server.socket.sendData("led", digitalRead(WIFI_LED)); // Send the current state of the LED
        }
        server.socket._socket.cleanupClients(); // Clean up disconnected WebSocket clients
    }
}

/**
 * @brief The main loop function (not used in this program).
 */
void loop()
{
    // This loop is intentionally left empty because the program logic is handled in the `setup()` function.
}