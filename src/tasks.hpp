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
#include <ESP32Servo.h>

/**
 * @brief Function to verify sensors
 */

void verifySensorsTask(void *pvParameters)
{
    while (true)
    {
        static unsigned long lastCheck = 0;
        if (millis() - lastCheck > 1000)
        {
            lastCheck = millis();

            Wire.beginTransmission(0x0D); // QMC5883L
            bool qmc5883l_ok = (Wire.endTransmission() == 0);

            Wire.beginTransmission(0x68); // MPU6050
            bool mpu6050_ok = (Wire.endTransmission() == 0);

            if (!qmc5883l_ok || !mpu6050_ok)
            {
                digitalWrite(FAILURE_LED, HIGH); // Turn on failure LED
                digitalWrite(WORKING_LED, LOW);  // Turn off working LED
            }
            else
            {
                digitalWrite(WORKING_LED, HIGH); // Turn on working LED
                digitalWrite(FAILURE_LED, LOW);  // Turn off failure LED
            }
        }
    }
}

/**
 * @brief Function to create an access point, set its IP address,
 * handle WiFi connections and limit the number of clients to one.
 */

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
        vTaskDelay(500 / portTICK_PERIOD_MS);       // Delay for a second
    }
}

/**
 * @brief Function to handle the web server
 *
 * This function creates a web server that serves a webpage and handles WebSocket connections.
 * It sends the current GPS coordinates and heading to the connected clients.
 */

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
        server.socket.sendData("lat", latStr);      // Send latitude to the web client
        server.socket.sendData("lon", lonStr);      // Send longitude to the web client
        server.socket.sendData("heading", headStr); // Send heading to the web client
        server.socket._socket.cleanupClients();     // Clean up disconnected WebSocket clients
        vTaskDelay(1000 / portTICK_PERIOD_MS);      // Delay for a second
    }
}

/**
 * @brief Function to get the GPS data from the GPS module
 */

void GPSTask(void *pvParameters)
{
    // Initialize GPS
    GPS gps(U1RXpin, U1TXpin, 19200); // RX=17, TX=18, 9600 baud

    while (true)
    {

        if (gps.getGPSData(latitude, longitude))
        {
            if (xSemaphoreTake(gpsMutex, portMAX_DELAY) == pdTRUE)
            {
                if (xSemaphoreTake(sensorGPSMutext, portMAX_DELAY) == pdTRUE)
                {
                    isGPSSensor = true; // Set the GPS sensor flag to true
                }
                else
                {
                    if (xSemaphoreTake(sensorGPSMutext, portMAX_DELAY) == pdTRUE)
                    {

                        isGPSSensor = false; // Set the GPS sensor flag to true
                    }
                }
                xSemaphoreGive(gpsMutex); // Release the mutex after getting GPS data
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay for a second
    }
}

/**
 * @brief Function to get the heading from the QMC5883L and MPU6050 sensors
 */

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

/**
 * @brief Function control the servomotor
 */
void ServoTask(void *pvParameters)
{

    Servo monServo;

    monServo.setPeriodHertz(50);          // Fréquence PWM pour les servos (50Hz)
    monServo.attach(servoPin, 500, 2400); // Signal min et max en microsecondes
    // 500–2400 µs sont les largeurs d’impulsion typiques pour 0–180°

    while (true)
    {
        // Safely read the shared servo angle variable
        if (xSemaphoreTake(servoMutex, portMAX_DELAY) == pdTRUE)
        {
            monServo.write(servoAngle); // Write the servo angle
            xSemaphoreGive(servoMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void MotorControl(void *pvParameters)
{
    const int channel = 3;
    const int frequence = 1000;
    const int resolution = 8;

    ledcSetup(channel, frequence, resolution);
    ledcAttachPin(motorPin, channel);

    while (true)
    {

        if (xSemaphoreTake(motorMutex, portMAX_DELAY) == pdTRUE)
        {
            ledcWrite(channel, motorVelocity);
            xSemaphoreGive(motorMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif // TASKS_HPP