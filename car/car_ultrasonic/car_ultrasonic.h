#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#define ULTRASONIC_TRIG_PIN 26
#define ULTRASONIC_ECHO_PIN 27
#define ULTRASONIC_SAMPLING_RATE 1000

// Function prototypes
void setupUltrasonic();
double getDistance();

#endif // ULTRASONIC_SENSOR_H
