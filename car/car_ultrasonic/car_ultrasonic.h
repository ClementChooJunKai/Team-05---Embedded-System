#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#define ULTRASONIC_TRIG_PIN 16
#define ULTRASONIC_ECHO_PIN 17
#define ULTRASONIC_SAMPLING_RATE 1000
#define NUM_READINGS 20

// Function prototypes
void setupUltrasonic();
double getDistance();
void ultrasonicHandler(uint32_t events);

#endif // ULTRASONIC_SENSOR_H
