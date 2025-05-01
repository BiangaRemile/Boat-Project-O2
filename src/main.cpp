#include <tasks.hpp>
#include "globals.hpp"

// Define the shared variables and mutex
SemaphoreHandle_t gpsMutex = NULL;
double latitude = 0.0;
double longitude = 0.0;


void setup() {
  Serial.begin(115200);

  gpsMutex = xSemaphoreCreateMutex();
  if (gpsMutex == NULL) {
    Serial.println("Failed to create mutex!");
    while(1); // Halt if mutex creation fails
  }

  // first core tasks

  xTaskCreatePinnedToCore(AccessPointTask, "AccessPoint", 6144, NULL, 2, NULL, 0); // Create Access point task
  xTaskCreatePinnedToCore(WebServerTask, "WebServer", 8192, NULL, 1, NULL, 0); // Create Webserver task
  xTaskCreatePinnedToCore(GPSTask, "GPS", 2048, NULL, 3, NULL, 0); // Create GPS task

  // second core tasks

}

void loop() {
  
}