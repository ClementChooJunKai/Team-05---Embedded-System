#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#define KP_L 0.05
#define KI_L 0.005
#define KD_L 0.0005

#define KP_R 0.05
#define KI_R 0.005
#define KD_R 0.0005

// Function prototypes
float pidUpdateLeft(float setpoint, float current_value);
float pidUpdateRight(float setpoint, float current_value);
#endif
