#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t gpsMutex;  // Mutex for GPS data
extern double latitude;            // Shared latitude
extern double longitude;           // Shared longitude

extern SemaphoreHandle_t headingMutex; // Mutex for heading data
extern double heading;            // Shared heading

extern SemaphoreHandle_t sensorGPSMutext;
extern bool isGPSSensor;