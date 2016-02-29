/** ***********************************************************************************
 * SerialPing native (hardware) Serial Example
 *
 * This example will only work with native (hardware) serial ports. This means
 * if you use Serial, you will not be able to use the serial monitor or communicate
 * over USB.
 * Some boards have additional hardware serial ports named Serial1, [Serial2, Serial3] 
 * e.g. Mega, Due and Leonardo (aka Pro Micro)
 * https://www.arduino.cc/en/Reference/Serial
 *
 * See https://github.com/OldGreyCells/SerialPing for full API
 *
 *********************************************************************************** */

//
// Include the SerialPing library
//
#include <SerialPing.h>

//
// Create the sonar instance using the constructor. Maximum distance is 3m.
//
SerialPing sonar(Serial1,300,SONAR_US_100);
// or using the ME007 sonar
//SerialPing sonar(serial1,300,SONAR_ME007_ULS);
//

// Used to delay each ping without using delay() function (a simple state machine)
unsigned long now = 0;           // Start time of current loop
unsigned long previousPing = 0;  // Store time of last ping


//
// The standard Arduino setup function.
//
void setup() {
  // Begin communication with the USB serial port for the serial monitor
  Serial.begin(115200);
  // Begin communication with the sonar. Note: Serial1, not Serial.
  Serial1.begin(9600);
  // Give the sonar a chance to start talking. A lower value may work, but
  // this is in setup, so not time critical.
  delay(1000);
}

//
// The standard Arduino loop function.
//
void loop() {
  // We want to measure approximately 5 times per second using the median
  // of 5 pings (i.e. there will be 25 pings per second).
  // Get the time we are entering the loop
  now = millis();
  // If more than 0.2 seconds since last ping, do another one.
  if (now - previousPing >= 200) {
    // Find the median of 5 rapid pings (in cm) and send result to the serial monitor.
    Serial.println(String(sonar.ping_median()) + "cm");
    // record the last time we pinged.
    previousPing = now;
  }
}
