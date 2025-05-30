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

extern SemaphoreHandle_t servoMutex; // Mutex for servo control
extern int servoAngle;             // Shared servo angle

extern SemaphoreHandle_t motorMutex;
extern int motorVelocity;