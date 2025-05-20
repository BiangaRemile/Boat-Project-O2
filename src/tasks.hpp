#ifndef TASKS_HPP
#define TASKS_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <gps.hpp>
#include <pins.hpp>
#include <wifi.hpp>
#include <webserver.hpp>
#include "globals.hpp"
#include "esp_wifi.h" // not esp_wifi_types.h only
#include "esp_wifi_types.h"
#include <heading.hpp>

void AccessPointTask(void *pvParameters)
{
    // Set up the access point
    const char *ssid = "Boat Remote";   // SSID of the access point
    const char *password = "123456789"; // Password for the access point

    IPAddress local_ip = create_access_point(ssid, password); // Create the access point

    pinMode(WIFI_LED, OUTPUT); // Set the WiFi LED pin as output
    Serial.print("IP Address: ");
    Serial.println(local_ip); // Print the IP address of the access point

    while (true)
    {
        digitalWrite(WIFI_LED, device_connected()); // Turn on the WiFi LED
        vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for a second
    }
}

void WebServerTask(void *pvParameters)
{
    // Set up the web server (not implemented in this snippet)
    // This task would typically handle incoming HTTP requests and serve web pages.
    WebServerManager server(80, "/ws"); // Create a web server on port 80 with WebSocket support
    while (true)
    {
        double currentLat, currentLon, currentHeading;

        // // Safely read shared variables
        if (xSemaphoreTake(gpsMutex, portMAX_DELAY) == pdTRUE)
        {
            currentLat = latitude;
            currentLon = longitude;
            xSemaphoreGive(gpsMutex);
        }

        if (xSemaphoreTake(headingMutex, portMAX_DELAY) == pdTRUE)
        {
            currentHeading = heading;
            xSemaphoreGive(headingMutex);
        }

        char latStr[16]; // Buffer for "±XXX.XXXXXX" (15 chars + null terminator)
        char lonStr[16];
        char headStr[8];

        snprintf(latStr, sizeof(latStr), "%.6f", currentLat);
        snprintf(lonStr, sizeof(lonStr), "%.6f", currentLon);
        snprintf(headStr, sizeof(headStr), "%.2f", currentHeading);

        // // Use the values (e.g., send to a web client)
        server.socket.sendData("lat", latStr);  // Send latitude to the web client
        server.socket.sendData("lon", lonStr);  // Send longitude to the web client
        server.socket.sendData("heading", headStr); // Send heading to the web client
        server.socket._socket.cleanupClients();     // Clean up disconnected WebSocket clients
        vTaskDelay(1000 / portTICK_PERIOD_MS);      // Delay for a second
    }
}

void GPSTask(void *pvParameters)
{
    // Initialize GPS
    GPS gps(U1RXpin, U1TXpin, 9600); // RX=17, TX=18, 9600 baud

    while (true)
    {
        if (gps.getGPSData(latitude, longitude))
        {
            isGPSSensor = true; // Set the GPS sensor flag to true
            if (xSemaphoreTake(gpsMutex, portMAX_DELAY) == pdTRUE)
            {
                xSemaphoreGive(gpsMutex); // Release the mutex after getting GPS data
            }
        } else
        {
            isGPSSensor = false; // Set the GPS sensor flag to false
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for a second
    }
}

void HeadingTask(void *pvParameters)
{

    initializationHeading();

    const float alpha = 0.98;

    while (true)
    {
        static unsigned long long lastTime = 0;
        static float headingCal = 0;

        pitchAndRoll(lastTime, headingCal, alpha);

        if (headingCal < 0)
        {
            headingCal += 360;
        }

        // Safely update the shared heading variable
        if (xSemaphoreTake(headingMutex, portMAX_DELAY) == pdTRUE)
        {
            heading = headingCal;
            xSemaphoreGive(headingMutex);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); 
    }
}

#endif // TASKS_HPP