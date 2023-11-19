#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEFT_ENCODER_PIN 2
#define RIGHT_ENCODER_PIN 3
#define WHEEL_ENCODER_SLOTS 20
#define WHEEL_DIAMETER 66          // Diameter in milimeter
#define WHEEL_CIRCUMFERENCE 207.35 // CIRCUMFERENCE in mm, derived from 66mm * PI
#define DISTANCE_IN_CM_PER_STEP 1.03675
#define POWER_PIN_2 4

int total_elapsed_time = 0;
float distance_travelled_in_cm_left = 0;
float distance_travelled_in_cm_right = 0;
struct repeating_timer timer;

// Timer callback used to create 1s time & calculate speed D/T
bool timer_callback(struct repeating_timer *t)
{
    total_elapsed_time++;
    printf("Total Elapsed Time: %ds\n",total_elapsed_time);

    float left_motor_speed = distance_travelled_in_cm_left/total_elapsed_time;
    float right_motor_speed = distance_travelled_in_cm_right/total_elapsed_time;
    printf("Left Motor Speed: %2fcm/s Right Motor Speed: %2fcm/s\n", left_motor_speed, right_motor_speed);
    return true;
}

// Interupt to calculate total distance travelled by left motor
/*void distance_callback_left_motor(uint gpio, uint32_t events)
{
    // static float distance_travelled_in_cm_left = 0;
    distance_travelled_in_cm_left += DISTANCE_IN_CM_PER_STEP;
    printf("Distance Travelled Left Motor: %2f cm\n", distance_travelled_in_cm_left);
    return;
}*/

// Interupt to calculate total distance travelled by right motor
/*void distance_callback_right_motor(uint gpio, uint32_t events)
{
    // static float distance_travelled_in_cm_right = 0;
    distance_travelled_in_cm_right += DISTANCE_IN_CM_PER_STEP;
    printf("Distance Travelled Right Motor: %2f cm\n", distance_travelled_in_cm_right);
    return;
}*/

/*void distance_callback(uint gpio, uint32_t events)
{
    if (gpio == LEFT_ENCODER_PIN){
        distance_travelled_in_cm_left += DISTANCE_IN_CM_PER_STEP;
        printf("Distance Travelled Left Motor: %2f cm\n", distance_travelled_in_cm_left);
    }else if (gpio == RIGHT_ENCODER_PIN){
        distance_travelled_in_cm_right += DISTANCE_IN_CM_PER_STEP;
        printf("Distance Travelled Right Motor: %2f cm\n", distance_travelled_in_cm_right);
    }
    return;
}*/

float calculateSpeed(char direction)
{
    switch (direction)
    {
    case 'L':
        return distance_travelled_in_cm_left / total_elapsed_time;
    case 'R':
        return distance_travelled_in_cm_right / total_elapsed_time;
    default:
        return 0;
    }
}

void setupWheelEncoder()
{
    // Repeating timer interupt of 1s, used to get the total elapsed time of program & speed of motor
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);
}

void leftWheelEncoderHandler()
{
    distance_travelled_in_cm_left += DISTANCE_IN_CM_PER_STEP;
    // printf("Distance Travelled Left Motor: %2f cm\n", distance_travelled_in_cm_left);
    return;
}

void rightWheelEncoderHandler()
{
    distance_travelled_in_cm_right += DISTANCE_IN_CM_PER_STEP;
    // printf("Distance Travelled Right Motor: %2f cm\n", distance_travelled_in_cm_right);
    return;
}

/*int main()
{
    stdio_init_all();
    sleep_ms(4000);
    gpio_init(POWER_PIN_2);
    gpio_set_dir(POWER_PIN_2, GPIO_OUT);
    gpio_put(POWER_PIN_2, 1);

    struct repeating_timer timer;

    // Repeating timer interupt of 1s, used to get the total elapsed time of program & speed of motor
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);

    // Interupt program for every high edge rise which correlates to 1 step on wheel encoder disc.
    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &distance_callback);
    gpio_set_irq_enabled_with_callback(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &distance_callback);


    while (1){
        tight_loop_contents();
    }
}*/