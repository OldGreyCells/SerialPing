# SerialPing

This library was first written for the [nearmiss-o-meter](https://github.com/OldGreyCells/nearmiss-o-meter) so serial sonars could be 'dropped' into the main Arduino code.

## FEATURES:
- Partially API compatible with the [NewPing](https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home) library.
- Currently supports two serial ultrasonic sonars:
  - http://en.dypsensor.com/products/&pmcId=42.html
  - https://www.google.co.uk/search?q=US-100+Ultrasonic+Distance+Sensor+Module
- Does not support the Maxbotix sonars as they use a peculiar hybrid serial RX + pin HIGH trigger.
- Works with hardware Serial and SoftwareSerial.
- Returns distances in whole cm by default, not uS.
- Can set a maximum distance where pings beyond that distance return 0cm.

## CONSTRUCTOR:
```SerialPing sonar(Stream, [, max_cm_distance [, sensor_id]])```
- Stream - Either HardwareSerial or SoftwareSerial.
- max_cm_distance - [Optional] Maximum distance you wish to sense. Default=500cm.
- sensor_id - [Optional] Either SONAR_US_100 (default) or SONAR_ME007_ULS

## METHODS:
- ```sonar.ping()``` - Send a ping and get the echo time (in cm) as a result. 
- ```sonar.ping_in()``` - Send a ping and get the distance in whole inches.
- ```sonar.ping_cm()``` - Send a ping and get the distance in whole centimeters.
- ```sonar.ping_median(iterations, timeout)``` - Do multiple pings (default=5), discard out of range pings, stop iterations if longer than timeout and return median in cm. 
- ```sonar.convert_in(cm)``` - Convert from cm to inches (rounds to nearest inch).
- ```sonar.convert_cm(cm)``` - convert cm to cm(!). For NewPing API compatability only.
