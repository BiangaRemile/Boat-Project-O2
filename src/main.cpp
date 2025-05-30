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

SemaphoreHandle_t servoMutex = NULL;
int servoAngle = 0;

SemaphoreHandle_t motorMutex = NULL;
int motorVelocity = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(WORKING_LED, OUTPUT);
  pinMode(FAILURE_LED, OUTPUT);

  gpsMutex = xSemaphoreCreateMutex();
  headingMutex = xSemaphoreCreateMutex();
  sensorGPSMutext = xSemaphoreCreateMutex();
  servoMutex = xSemaphoreCreateMutex();
  motorMutex = xSemaphoreCreateMutex();


  if ((gpsMutex == NULL) || (headingMutex == NULL) || (sensorGPSMutext == NULL) || (servoMutex == NULL) || (motorMutex == NULL))
  {
    Serial.println("Failed to create mutex!");
    digitalWrite(WORKING_LED, HIGH);
    while (1)
      ; // Halt if mutex creation fails
  }

  // first core tasks

  xTaskCreatePinnedToCore(AccessPointTask, "AccessPoint", 6144, NULL, 2, NULL, 0); // Create Access point task
  xTaskCreatePinnedToCore(WebServerTask, "WebServer", 8192, NULL, 1, NULL, 0);     // Create Webserver task
  //xTaskCreatePinnedToCore(GPSTask, "GPS", 2048, NULL, 6, NULL, 0);                 // Create GPS task
  xTaskCreatePinnedToCore(HeadingTask, "Heading", 4096, NULL, 4, NULL, 0);         // Create Heading task

  xTaskCreatePinnedToCore(verifySensorsTask, "Verification", 2048, NULL, 1, NULL, 1);  // Create Servo task
  xTaskCreatePinnedToCore(ServoTask, "Servo", 4096, NULL, 6, NULL, 1);  // Create Servo task
  xTaskCreatePinnedToCore(MotorControl, "Motor control", 4096, NULL, 7, NULL, 1); // control dc motor task

  // second core tasks
}

void loop()
{
  
  
} 