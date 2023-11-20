#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "car_wheel_driver.h"

void motor_set_left_speed(float motor_speed)
{
    uint left_motor_slice_num = pwm_gpio_to_slice_num(ENA);
    // [MOTOR] Setting max PWM for left motor (Channel A)
    pwm_set_chan_level(left_motor_slice_num, PWM_CHAN_A, 12500 * motor_speed);
    pwm_set_enabled(left_motor_slice_num, true);
    return;
}

void motor_set_right_speed(float motor_speed)
{
    uint right_motor_slice_num = pwm_gpio_to_slice_num(ENB);
     // [MOTOR] Setting max PWM for right motor (Channel B)
    pwm_set_chan_level(right_motor_slice_num, PWM_CHAN_B, 12500 * motor_speed);
    pwm_set_enabled(right_motor_slice_num, true);
}

void motor_set_speed(float left_motor_speed, float right_motor_speed)
{
    motor_set_left_speed(left_motor_speed);
    motor_set_right_speed(right_motor_speed);
}

int motor_initialize()
{

    // [MOTOR] Initializing GPIO for Left Motor
    gpio_init(IN1);
    gpio_init(IN2);
    gpio_init(ENA);

    // [MOTOR] Initializing GPIO for Right Motor
    gpio_init(IN3);
    gpio_init(IN4);
    gpio_init(ENB);

    // [MOTOR] Configuring GPIO Pins as Output
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_set_dir(IN2, GPIO_OUT);
    gpio_set_dir(IN3, GPIO_OUT);
    gpio_set_dir(IN4, GPIO_OUT);
    gpio_set_dir(ENA, GPIO_OUT);
    gpio_set_dir(ENB, GPIO_OUT);

    // [MOTOR] Configuring ENA & ENB as PWM
    gpio_set_function(ENA, GPIO_FUNC_PWM);
    gpio_set_function(ENB, GPIO_FUNC_PWM);

    uint left_motor_slice_num = pwm_gpio_to_slice_num(ENA);
    uint right_motor_slice_num = pwm_gpio_to_slice_num(ENB);

    // [MOTOR] Configure PWM to be of clock divisor 100 & set PWM width to be 12500
    pwm_set_clkdiv(left_motor_slice_num, 100);
    pwm_set_wrap(left_motor_slice_num, 12500);
    pwm_set_clkdiv(right_motor_slice_num, 100);
    pwm_set_wrap(right_motor_slice_num, 12500);

    return 0;
}

// [MOTOR] Spins Left & Right Motor spins forward.
int motor_forward(float left_motor_speed, float right_motor_speed)
{
    motor_set_speed(left_motor_speed, right_motor_speed);
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);

    return 0;
}

// [MOTOR] Left & Right Motor spins backwards.
int motor_reverse(float left_motor_speed, float right_motor_speed)
{
    motor_set_speed(left_motor_speed, right_motor_speed);    
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);

    return 0;
}

// [MOTOR] Right Motor stops, Left Motor goes forward
int motor_forward_right(float left_motor_speed, float right_motor_speed)
{
    motor_set_speed(left_motor_speed, right_motor_speed);
    gpio_put(IN1, 0);
    gpio_put(IN2, 1);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);

    return 0;
}

int motor_reverse_right(float left_motor_speed, float right_motor_speed)
{
    motor_set_speed(left_motor_speed, right_motor_speed);
    gpio_put(IN1, 1);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);

    return 0;
}

// [MOTOR] Right Motor goes forward, Left Motor stops
int motor_forward_left(float left_motor_speed, float right_motor_speed)
{
    motor_set_speed(left_motor_speed, right_motor_speed);
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 1);

    return 0;
}

int motor_reverse_left(float left_motor_speed, float right_motor_speed)
{
    motor_set_speed(left_motor_speed, right_motor_speed);
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 1);
    gpio_put(IN4, 0);

    return 0;
}

// [MOTOR] Both Motor Stops
int motor_stop()
{
    gpio_put(IN1, 0);
    gpio_put(IN2, 0);
    gpio_put(IN3, 0);
    gpio_put(IN4, 0);

    return 0;
}