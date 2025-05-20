#include <tasks.hpp>
#include "globals.hpp"

// Define the GPS shared variables and mutex
SemaphoreHandle_t gpsMutex = NULL;
double latitude = 0.0;
double longitude = 0.0;

SemaphoreHandle_t headingMutex = NULL;
double heading = 0.0;

SemaphoreHandle_t sensorGPSMutext = NULL;
bool isGPSSensor = false;

void setup()
{
  Serial.begin(115200);

  pinMode(WORKING_LED, OUTPUT);
  pinMode(FAILURE_LED, OUTPUT);

  gpsMutex = xSemaphoreCreateMutex();
  headingMutex = xSemaphoreCreateMutex();
  sensorGPSMutext = xSemaphoreCreateMutex();
  if ((gpsMutex == NULL) || (headingMutex == NULL) || (sensorGPSMutext == NULL))
  {
    Serial.println("Failed to create mutex!");
    digitalWrite(WORKING_LED, HIGH);
    while (1)
      ; // Halt if mutex creation fails
  }

  // first core tasks

  xTaskCreatePinnedToCore(AccessPointTask, "AccessPoint", 6144, NULL, 2, NULL, 0); // Create Access point task
  xTaskCreatePinnedToCore(WebServerTask, "WebServer", 8192, NULL, 1, NULL, 0);     // Create Webserver task
  xTaskCreatePinnedToCore(GPSTask, "GPS", 2048, NULL, 3, NULL, 0);                 // Create GPS task
  xTaskCreatePinnedToCore(HeadingTask, "Heading", 4096, NULL, 4, NULL, 0);         // Create Heading task

  // second core tasks
}

void loop()
{
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000)
  {
    lastCheck = millis();

    Wire.beginTransmission(0x0D); // QMC5883L
    bool qmc5883l_ok = (Wire.endTransmission() == 0);

    Wire.beginTransmission(0x68); // MPU6050
    bool mpu6050_ok = (Wire.endTransmission() == 0);

    if (!qmc5883l_ok || !mpu6050_ok) {
      digitalWrite(FAILURE_LED, HIGH); // Turn on failure LED
      digitalWrite(WORKING_LED, LOW); // Turn off working LED
    } else {
      digitalWrite(WORKING_LED, HIGH); // Turn on working LED
      digitalWrite(FAILURE_LED, LOW); // Turn off failure LED
    }
  }
  
}