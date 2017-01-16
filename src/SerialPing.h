/**
 * SerialPing Library - v0.2 - 17/01/2017
 *
 * AUTHOR/LICENSE:
 * Created by @greycells - oldgreycells@gmail.com
 * Copyright 2016 License: The MIT License (MIT)
 *
 * LINKS:
 * Project home: https://github.com/OldGreyCells/SerialPing
 *
 * DISCLAIMER:
 * This software is furnished "as is", without technical support, and with no 
 * warranty, express or implied, as to its usefulness for any purpose.
 *
 * BACKGROUND:
 * This library was first written for the nearmiss-o-meter (https://github.com/OldGreyCells/nearmiss-o-meter)
 * so serial sonars could be 'dropped' into the main Arduino code.
 *
 * FEATURES:
 * - Partially API compatible with the NewPing library.
 * - Currently supports two serial ultrasonic sonars:
 *   - http://en.dypsensor.com/products/&pmcId=42.html
 *   - https://www.google.co.uk/search?q=US-100+Ultrasonic+Distance+Sensor+Module
 * - Does not support the Maxbotix sonars as they use a peculiar 
 *   hybrid serial RX + pin HIGH trigger.
 * - Works with hardware Serial and SoftwareSerial.
 * - Returns distances in whole cm by default, not uS.
 * - Can set a maximum distance where pings beyond that distance return 0cm.
 *
 * CONSTRUCTOR:
 *   SerialPing sonar(Stream, [, max_cm_distance [, sensor_id]])
 *     Stream - Either HardwareSerial or SoftwareSerial.
 *     max_cm_distance - [Optional] Maximum distance you wish to sense. Default=500cm.
 *     sensor_id - [Optional] Either SONAR_US_100 (default) or SONAR_ME007_ULS
 *
 * METHODS:
 *   sonar.ping() - Send a ping and get the echo time (in cm) as a result. 
 *   sonar.ping_in() - Send a ping and get the distance in whole inches.
 *   sonar.ping_cm() - Send a ping and get the distance in whole centimeters.
 *   sonar.ping_median(iterations) - Do multiple pings (default=5), discard out of range pings and return median in cm. 
 *   sonar.convert_in(cm) - Convert from cm to inches (rounds to nearest inch).
 *   sonar.convert_cm(cm) - convert cm to cm(!). For NewPing API compatability only.
 */
#ifndef SerialPing_H
#define SerialPing_H
#include <Arduino.h>
#define NO_ECHO 0
#define PING_OVERHEAD 1500
#define MAX_SENSOR_DISTANCE 500  // Maximum sensor distance can be as high as 500cm, no reason to wait for ping longer than sound takes to 
#define PING_MEDIAN_DELAY 100 //29000  // Microsecond delay between pings in the ping_median method. Default=29000
#define SONAR_US_100 1
#define SONAR_ME007_ULS 2


class SerialPing {
  public:
    SerialPing(Stream& s, unsigned int max_cm_distance = MAX_SENSOR_DISTANCE, unsigned int sensor_id = SONAR_US_100);
    /**
     * Return cm from single ping().
     */
    unsigned int ping();
    /**
     * Return median cm from <it> number of pings within <timeout>.
     */
    unsigned int ping_median(uint8_t it = 5, uint16_t timeout = 200);
    /**
     * Alias for ping() for NewPing API compatibility.
     */
    unsigned int ping_cm();
    /**
     * Convert cm to inches.
     */
    unsigned int ping_in();
    /**
     * Convert cm to cm(!).
     * Solely for NewPing API compatibility
     */
    unsigned int convert_cm(unsigned int cm);
    /**
     * Returns whole inches from cm
     */
    unsigned int convert_in(unsigned int cm);
    
  private:
    Stream& serial;
    unsigned int pingUS100();
    unsigned int pingME007ULS();
    unsigned int maxDistance = MAX_SENSOR_DISTANCE;
    //unsigned long maxWait = 70000;
    unsigned int sensorId = SONAR_US_100;
    unsigned long pingStart = 0;
    unsigned long pingMedianDelay = PING_MEDIAN_DELAY;
    int byteCounter = 0;
    byte dataH;
    byte dataL;
};

#endif

