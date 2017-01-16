#include "Arduino.h"
#include "SerialPing.h"

SerialPing::SerialPing(Stream& s, unsigned int max_cm_distance, unsigned int sensor_id): serial(s) {
  maxDistance = min(max_cm_distance, (unsigned int) MAX_SENSOR_DISTANCE);
  sensorId = sensor_id;
  //maxWait=PING_OVERHEAD+(maxDistance*58);
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
 * Return median cm from <it> number of pings within <timeout>.
 * Used with kind permission of Tim Eckel (creator of NewPing)
 */
unsigned int SerialPing::ping_median(uint8_t it, uint16_t timeout) {
  unsigned int cm[it], last;
  uint8_t j, i = 0;
  unsigned long t;
  unsigned long start = millis();

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

    //Break loop if (usually first) ping takes too long
    if ( millis() - start > timeout ) break;
    if (i < it && micros() - t < PING_MEDIAN_DELAY) 
      delay((PING_MEDIAN_DELAY + t - micros()) / 1000); // Millisecond delay between pings.
  }
  return (cm[i >> 1]); // Return the ping distance median.
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
    byte serialData[3];
    //Clear down any byte(s) left in the buffer
    while (serial.available()) {
      byte b = serial.read();
    }
    serial.write(0x55);
    serial.setTimeout(95);
    if ( serial.readBytes(serialData, 2) > 0 ) {
        int cm = ((serialData[0] * 256) + serialData[1]) / 10;
        memset(serialData, 0, sizeof(serialData));   // Clear contents of Buffer
        serial.flush();
        return (cm > maxDistance) ? NO_ECHO : cm;
    } else {
        return NO_ECHO;
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
      if ( millis() - pingStart > 95 ) { 
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
                Serial.println(dataH, HEX);
                break;
              case 2:
                dataL = b;
                Serial.println(dataL, HEX);
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

