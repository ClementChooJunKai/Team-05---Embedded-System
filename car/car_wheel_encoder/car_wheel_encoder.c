#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEFT_ENCODER_PIN 2
#define RIGHT_ENCODER_PIN 3
#define WHEEL_ENCODER_SLOTS 20
#define WHEEL_DIAMETER 66          // Diameter in millimeter
#define WHEEL_CIRCUMFERENCE 207.35 // CIRCUMFERENCE in mm, derived from 66mm * PI
#define DISTANCE_IN_CM_PER_STEP 1.03675
#define POWER_PIN_2 4

int total_elapsed_time = 0;
struct repeating_timer timer;

volatile int left_encoder_count = 0;
volatile int right_encoder_count = 0;

volatile float left_motor_speed;
volatile float right_motor_speed;

float calculateSpeed(char direction)
{
    int encoder_count;
    float distance_travelled_in_cm;

    switch (direction)
    {
    case 'L':
        encoder_count = left_encoder_count;
        distance_travelled_in_cm = DISTANCE_IN_CM_PER_STEP * encoder_count;
        left_encoder_count = 0; // Reset the count
        break;
    case 'R':
        encoder_count = right_encoder_count;
        distance_travelled_in_cm = DISTANCE_IN_CM_PER_STEP * encoder_count;
        right_encoder_count = 0; // Reset the count
        break;
    default:
        return 0;
    }

    return distance_travelled_in_cm;
}

// Timer callback used to create 1s time & calculate speed D/T
bool timer_callback(struct repeating_timer *t)
{
    total_elapsed_time++;
    printf("Total Elapsed Time: %ds\n", total_elapsed_time);

    left_motor_speed = calculateSpeed('L');
    right_motor_speed = calculateSpeed('R');
    printf("Left Motor Speed: %.2f cm/s, Right Motor Speed: %.2f cm/s\n", left_motor_speed, right_motor_speed);

    return true;
}

void setupWheelEncoder()
{
    // Set up GPIO pins for left and right wheel encoders
    gpio_init(LEFT_ENCODER_PIN);
    gpio_init(RIGHT_ENCODER_PIN);

    gpio_set_dir(LEFT_ENCODER_PIN, GPIO_IN);
    gpio_set_dir(RIGHT_ENCODER_PIN, GPIO_IN);

    // Repeating timer interrupt of 1s, used to get the total elapsed time of the program & speed of motor
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);
}

void leftWheelEncoderHandler()
{
    left_encoder_count++;
}

void rightWheelEncoderHandler()
{
    right_encoder_count++;
}

float getLeftMotorSpeed(){
    return left_motor_speed;
}

float getRightMotorSpeed(){
    return right_motor_speed;
}
