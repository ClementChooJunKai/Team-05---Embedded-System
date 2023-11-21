#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#define KP 0.01
#define KI 0.001
#define KD 0.0001

#define KP_L 0.02
#define KI_L 0.002
#define KD_L 0.0002

#define KP_R 0.01
#define KI_R 0.001
#define KD_R 0.0001

// Function prototypes
float pidUpdateLeft(float setpoint, float current_value);
float pidUpdateRight(float setpoint, float current_value);
float pidUpdateSpeed(float setpoint, float current_value);
void resetIntegral();
void resetLeftIntegral();
void resetRightIntegral();
#endif
