#include "Arduino.h"
#include "SerialPing.h"

SerialPing::SerialPing(Stream& s, unsigned int max_cm_distance, unsigned int sensor_id): serial(s) {
  maxDistance = min(max_cm_distance, (unsigned int) MAX_SENSOR_DISTANCE);
  sensorId = sensor_id;
}

/**
 * Return cm from single ping().
 */
unsigned int SerialPing::ping() {
  switch(sensorId) {
    case(SONAR_US_100):
      return pingUS100();
      break;
    case(SONAR_ME007_ULS):
      return pingME007ULS();
      break;
    default:
      return NO_ECHO;
  }
}

/**
 * Return median cm from <it> number of pings.
 * Used with kind permission of Tim Eckel (creator of NewPing)
 */
unsigned int SerialPing::ping_median(uint8_t it) {
  unsigned int cm[it], last;
  uint8_t j, i = 0;
  unsigned long t;
  //unsigned int actualLoops = 0;
  cm[0] = NO_ECHO;

  while (i < it) {
    t = micros();  // Start ping timestamp.
    last = ping(); // Send ping.

    if (last != NO_ECHO) {       // Ping in range, include as part of median.
      if (i > 0) {               // Don't start sort till second ping.
        for (j = i; j > 0 && cm[j - 1] < last; j--) // Insertion sort loop.
          cm[j] = cm[j - 1];     // Shift ping array to correct position for sort insertion.
      } else j = 0;              // First ping is sort starting point.
      cm[j] = last;              // Add last ping to array in sorted position.
      i++;                       // Move to next ping.
    } else it--;                 // Ping out of range, skip and don't include as part of median.

    if (i < it && micros() - t < PING_MEDIAN_DELAY)
      delay((PING_MEDIAN_DELAY + t - micros()) / 1000); // Millisecond delay between pings.
  }
  return (cm[it >> 1]); // Return the ping distance median.
}


/**
 * Alias for ping() for NewPing API compatibility.
 */
unsigned int SerialPing::ping_cm() {
  return ping();
}
/**
 * Convert cm to inches.
 */
unsigned int SerialPing::ping_in() {
  return convert_in(ping());
}
/**
 * Convert cm to cm(!). Solely for NewPing API compatibility.
 */
unsigned int SerialPing::convert_cm(unsigned int cm) {
  return cm;
}
/**
 * Returns whole inches from cm
 */
unsigned int SerialPing::convert_in(unsigned int cm) {
  //Add 0.5 to cm  so we can round without using float.
  return (int)(cm/2.54)+0.5;
}



unsigned int SerialPing::pingUS100() {
  boolean waitingForFrame = false;
  boolean haveStartOfFrame = false;
  //Serial.print("ping: ");
  //if (!waitingForFrame) {
  pingStart = millis();
  serial.write(0x55);
  waitingForFrame = true;
  while (waitingForFrame) {
    if ( millis() - pingStart > 70) {
      //Serial.println("Waited too long for echo");
      waitingForFrame = false;
      return NO_ECHO;
    }
    while (serial.available()) {
      byte b = serial.read();
      if ( b != -1) {
        switch (byteCounter) {
          case 0:
            dataH = b;
            break;
          case 1:
            dataL = b;
            int cm = ((dataH * 256) + dataL) / 10;
            waitingForFrame = false;
            byteCounter = 0;
            serial.flush();
            return (cm > maxDistance) ? NO_ECHO : cm;
            //return cm;
        }
        byteCounter++;
        if (byteCounter > 50 ) {
          waitingForFrame = false;
          return NO_ECHO;
        }
      }
    }
  }
}

unsigned int SerialPing::pingME007ULS() {
  boolean waitingForFrame = false;
  boolean haveStartOfFrame = false;
  //Serial.print("ping: ");
  if (!waitingForFrame) {
    pingStart = millis();
    serial.write(1);
    waitingForFrame = true;
    while (waitingForFrame) {
      //Serial.println(millis() - pingStart);
      if ( millis() - pingStart > 70) {
        //Serial.println("Waited too long for echo");
        waitingForFrame = false;
        return NO_ECHO; 
      }
      if (serial.available()) {
        byte b = serial.read();
        if ( b != -1) {
          if ( b == 0xFF ) {
            byteCounter = 0;
            haveStartOfFrame = true;
          }
          if ( haveStartOfFrame ) {
            switch (byteCounter) {
              case 1:
                dataH = b;
                //Serial.println(dataH, HEX);
                break;
              case 2:
                dataL = b;
                //Serial.println(dataL, HEX);
                break;
              case 3:
                //tempH = b;
                break;
              case 4:
                //tempL = b;
                break;
              case 5:
                int cm = (int)word(dataH, dataL) / 10;
                waitingForFrame = false;
                serial.flush();
                return (cm > maxDistance) ? NO_ECHO : cm;
                //Serial.println(last);
                //byte centigrade = (int)((tempH & 0x7F)*256+tempL)/10;
                //waitingForFrame = false;
                //haveStartOfFrame = false;
            }
            byteCounter++;
            if (byteCounter > 20 ) {
              waitingForFrame = false;
              return NO_ECHO;
            }
          }
        }
      }
    }
  }
}

