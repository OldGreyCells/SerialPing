/** **********************************************************************************
 * SerialPing SoftwareSerial Example
 *
 * This example will work with any board that supports SoftwareSerial.
 * https://www.arduino.cc/en/Reference/SoftwareSerial
 *
 * See https://github.com/OldGreyCells/SerialPing for full API
 *
 *********************************************************************************** */
 

//
// Include the SoftwareSerial library
//
#include <SoftwareSerial.h>

//
// Include the SerialPing library
//
#include <SerialPing.h>

//
// Define which pins we are goinf to use for softwareSerial
//
const byte RX_PIN = 10;
const byte TX_PIN = 11;

//
// Create the software serial instance.
//
SoftwareSerial softSerial(RX_PIN,TX_PIN); //RX TX

//
// Create the sonar instance using the constructor. Maximum distance is 3m.
//
SerialPing sonar(softSerial,300,SONAR_US_100);
// or using the ME007 sonar
//SerialPing sonar(softSerial,300,SONAR_ME007_ULS);
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
  // Begin communication with the sonar via softSerial.
  softSerial.begin(9600);
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
