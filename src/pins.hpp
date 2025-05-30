// If __PINS__ is not defined, define it. This prevents the contents of this file 
// from being included multiple times in the same compilation unit (header guard).
#ifndef __PINS__
#define __PINS__

// Define pin mappings for the microcontroller.
// These constants are used to associate specific functionality with hardware pins.

#define WIFI_LED 40 // Pin 46 is assigned to control the WiFi status LED.
#define WORKING_LED 41 // Pin 45 is assigned to control the working status LED.
#define FAILURE_LED 42 // Pin 44 is assigned to control the failure status LED.

#define I2C_SDA 5 // SDA pin for I2C communication
#define I2C_SCL 4 // SCL pin for I2C communication
#define U1RXpin 18 // Pin 17 is assigned for RX1 (receive) functionality.
#define U1TXpin 17 // Pin 16 is assigned for TX1 (receive) functionality.

#define servoPin 38 // Pin 38 is assigned for the servo motor control signal.
#define motorPin 37 // Pin to control motor with PWM

// Close the conditional inclusion block.
#endif