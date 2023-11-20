#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "car_pid_controller.h"

float integral_left = 0.0;
float prev_error_left = 0.0;

float integral_right = 0.0;
float prev_error_right = 0.0;

float pidUpdateLeft(float setpoint, float current_value) {

    float error = setpoint - current_value;
    integral_left += error;
    float derivative = error - prev_error_left;

    float output = KP_L * error + KI_L * integral_left + KD_L * derivative;

    prev_error_left = error;
    return output;
}

float pidUpdateRight(float setpoint, float current_value) {

    float error = setpoint - current_value;
    integral_right += error;
    float derivative = error - prev_error_right;

    float output = KP_R * error + KI_R * integral_right + KD_R * derivative;

    prev_error_right = error;
    return output;
}
