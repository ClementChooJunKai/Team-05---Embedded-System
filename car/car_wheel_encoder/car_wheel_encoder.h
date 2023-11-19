#ifndef WHEEL_ENCODER_H
#define WHEEL_ENCODER_H

#define LEFT_ENCODER_PIN 2
#define RIGHT_ENCODER_PIN 3
#define WHEEL_ENCODER_SLOTS 20
#define WHEEL_DIAMETER 66          // Diameter in milimeter
#define WHEEL_CIRCUMFERENCE 207.35 // CIRCUMFERENCE in mm, derived from 66mm * PI
#define DISTANCE_IN_CM_PER_STEP 1.03675
#define POWER_PIN_2 4

// Function prototypes
void setupWheelEncoder();
void leftWheelEncoderHandler();
void rightWheelEncoderHandler();
float getLeftMotorSpeed();
float getRightMotorSpeed();


#endif
