// If __PINS__ is not defined, define it. This prevents the contents of this file 
// from being included multiple times in the same compilation unit (header guard).
#ifndef __PINS__
#define __PINS__

// Define pin mappings for the microcontroller.
// These constants are used to associate specific functionality with hardware pins.

#define WIFI_LED 46 // Pin 46 is assigned to control the WiFi status LED.
#define I2C_SDA 5 // SDA pin for I2C communication
#define I2C_SCL 4 // SCL pin for I2C communication
#define U1RXpin 17 // Pin 17 is assigned for RX1 (receive) functionality.
#define U1TXpin 18 // Pin 16 is assigned for RX2 (receive) functionality.

// Close the conditional inclusion block.
#endif