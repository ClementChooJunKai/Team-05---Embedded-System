#include <stdio.h>                  // Standard I/O library
#include "pico/stdlib.h"            // Pico standard library
#include "hardware/gpio.h"          // Hardware GPIO library
#include "car_wheel_encoder.h"      // Include the header file

// Global variables
int total_elapsed_time = 0;         // Total elapsed time in seconds
struct repeating_timer timer;       // Repeating timer instance

volatile int left_encoder_count = 0;    // Counter for left wheel encoder ticks
volatile int right_encoder_count = 0;   // Counter for right wheel encoder ticks

volatile float left_motor_speed;    // Speed of the left motor
volatile float right_motor_speed;   // Speed of the right motor

// Function to calculate speed based on encoder count and direction
float calculateSpeed(char direction)
{
    int encoder_count;
    float distance_travelled_in_cm;

    switch (direction)
    {
    case 'L':
        encoder_count = left_encoder_count;
        distance_travelled_in_cm = DISTANCE_IN_CM_PER_STEP * encoder_count;
        left_encoder_count = 0; // Reset the count after calculation
        break;
    case 'R':
        encoder_count = right_encoder_count;
        distance_travelled_in_cm = DISTANCE_IN_CM_PER_STEP * encoder_count;
        right_encoder_count = 0; // Reset the count after calculation
        break;
    default:
        return 0; // Return 0 if direction is invalid
    }

    return distance_travelled_in_cm; // Return the calculated distance
}

// Timer callback function to calculate speed and total elapsed time
bool timer_callback(struct repeating_timer *t)
{
    total_elapsed_time++; // Increment total elapsed time

    // Calculate speed based on encoder counts and direction
    left_motor_speed = calculateSpeed('L');
    right_motor_speed = calculateSpeed('R');

    return true;
}

// Setup function to initialize wheel encoders and the repeating timer
void setupWheelEncoder()
{
    // Initialize GPIO pins for left and right wheel encoders
    gpio_init(LEFT_ENCODER_PIN);
    gpio_init(RIGHT_ENCODER_PIN);
    gpio_set_dir(LEFT_ENCODER_PIN, GPIO_IN);
    gpio_set_dir(RIGHT_ENCODER_PIN, GPIO_IN);

    // Create a repeating timer interrupt of 1 second
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);
}

// Interrupt handler for left wheel encoder
void leftWheelEncoderHandler()
{
    left_encoder_count++; // Increment left encoder count on detection
}

// Interrupt handler for right wheel encoder
void rightWheelEncoderHandler()
{
    right_encoder_count++; // Increment right encoder count on detection
}

// Function to retrieve left motor speed
float getLeftMotorSpeed()
{
    return left_motor_speed; // Return left motor speed
}

// Function to retrieve right motor speed
float getRightMotorSpeed()
{
    return right_motor_speed; // Return right motor speed
}
