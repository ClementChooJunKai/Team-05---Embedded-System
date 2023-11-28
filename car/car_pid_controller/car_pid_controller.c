#include <stdio.h>                  // Standard I/O library
#include "pico/stdlib.h"            // Pico standard library
#include "hardware/gpio.h"          // Hardware GPIO library
#include "pico/time.h"              // Pico time library
#include "car_pid_controller.h"     // Include the header file for PID controller constants

// Initialize variables for PID control for overall speed
float integral = 0.0;
float prev_error = 0.0;

// Initialize variables for PID control for left wheel speed
float integral_left = 0.0;
float prev_error_left = 0.0;

// Initialize variables for PID control for right wheel speed
float integral_right = 0.0;
float prev_error_right = 0.0;

// Function to update PID for left wheel speed control
float pidUpdateLeft(float setpoint, float current_value) {
    float error = setpoint - current_value;
    integral_left += error;
    float derivative = error - prev_error_left;

    // Calculate PID output for left wheel speed control
    float output = KP_L * error + KI_L * integral_left + KD_L * derivative;

    prev_error_left = error;
    return output;
}

// Function to update PID for right wheel speed control
float pidUpdateRight(float setpoint, float current_value) {
    float error = setpoint - current_value;
    integral_right += error;
    float derivative = error - prev_error_right;

    // Calculate PID output for right wheel speed control
    float output = KP_R * error + KI_R * integral_right + KD_R * derivative;

    prev_error_right = error;
    return output;
}

// Function to update PID for overall speed control
float pidUpdateSpeed(float setpoint, float current_value){
    float error = setpoint - current_value;
    integral += error;
    float derivative = error - prev_error;

    // Calculate PID output for overall speed control
    float output = KP * error + KI * integral + KD * derivative;

    prev_error = error;
    return output;
}

// Function to reset the integral component for overall speed control PID
void resetIntegral(){
    integral = 0.0;
}

// Function to reset the integral component for left wheel speed control PID
void resetLeftIntegral(){
    integral_left = 0.0;
}

// Function to reset the integral component for right wheel speed control PID
void resetRightIntegral(){
    integral_right = 0.0;
}
