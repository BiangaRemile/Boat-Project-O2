#ifndef TASKS_HPP
#define TASKS_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <gps.hpp>
#include <pins.hpp>
#include <wifi.hpp>
#include <webserver.hpp>
#include "globals.hpp"

void AccessPointTask(void *pvParameters)
{
    // Set up the access point
    const char *ssid = "Boat Remote";   // SSID of the access point
    const char *password = "123456789"; // Password for the access point

    IPAddress local_ip = create_access_point(ssid, password); // Create the access point

    Serial.print("IP Address: ");
    Serial.println(local_ip); // Print the IP address of the access point

    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for a second
    }
}

void WebServerTask(void *pvParameters)
{
    // Set up the web server (not implemented in this snippet)
    // This task would typically handle incoming HTTP requests and serve web pages.
    WebServerManager server(80, "/ws"); // Create a web server on port 80 with WebSocket support
    while (true)
    {
        double currentLat, currentLon;

        // Safely read shared variables
        if (xSemaphoreTake(gpsMutex, portMAX_DELAY) == pdTRUE)
        {
            currentLat = latitude;
            currentLon = longitude;
            xSemaphoreGive(gpsMutex);
        }

        char latStr[16]; // Buffer for "±XXX.XXXXXX" (15 chars + null terminator)
        char lonStr[16];
        
        snprintf(latStr, sizeof(latStr), "%.6f", currentLat);
        snprintf(lonStr, sizeof(lonStr), "%.6f", currentLon);

        // Use the values (e.g., send to a web client)
        server.socket.sendData("lat", latStr); // Send latitude to the web client
        server.socket.sendData("lon", lonStr); // Send longitude to the web client
        server.socket._socket.cleanupClients(); // Clean up disconnected WebSocket clients
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for a second
    }
}

void GPSTask(void *pvParameters)
{
    // Initialize GPS
    GPS gps(U1RXpin, U1TXpin, 9600); // RX=17, TX=18, 9600 baud
    Serial.println("GPS Module Test");

    if (xSemaphoreTake(gpsMutex, portMAX_DELAY) == pdTRUE)
    {

        while (true)
        {
            if (gps.getGPSData(latitude, longitude))
            {
                xSemaphoreGive(gpsMutex); // Release the mutex after getting GPS data
            }
            else
            {
            }
            vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for a second
        }
    }
}

#endif // TASKS_HPP