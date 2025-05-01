#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t gpsMutex;  // Mutex for GPS data
extern double latitude;            // Shared latitude
extern double longitude;           // Shared longitude