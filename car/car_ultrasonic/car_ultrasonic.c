#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "car_ultrasonic.h"

// #define ULTRASONIC_TRIG_PIN 26
// #define ULTRASONIC_ECHO_PIN 27
// #define ULTRASONIC_SAMPLING_RATE 1000
// #define ULTRASONIC_POWER_PIN 3

static absolute_time_t start_time;
static absolute_time_t end_time;
//static bool rising_edge = true;
int32_t durations_us[NUM_READINGS];


void ultrasonicHandler(uint32_t events)
{
    static int index = 0;
    if (events == GPIO_IRQ_EDGE_RISE) {
        start_time = get_absolute_time(); // Record the time of a rising edge
    } else if (events == GPIO_IRQ_EDGE_FALL) {
        end_time = get_absolute_time(); // Record the time of a falling edge
        durations_us[index] = absolute_time_diff_us(start_time, end_time);
        index++;
        if (index >= NUM_READINGS){
            index = 0;
        }
    }
}

void setupUltrasonic() {
    // gpio_init(ULTRASONIC_POWER_PIN); //Initializes the GPIO pin as power.
    // gpio_set_dir(ULTRASONIC_POWER_PIN, GPIO_OUT); // Set the pin as an output
    // gpio_put(ULTRASONIC_POWER_PIN, true); // Set the pin high to output 3.3V to supply power to ultrasonic

    gpio_init(ULTRASONIC_TRIG_PIN); // Initialize the trigger pin
    gpio_set_dir(ULTRASONIC_TRIG_PIN, GPIO_OUT); // Set the trigger pin as an output
    gpio_init(ULTRASONIC_ECHO_PIN); // Initialize the ECHO pin
    gpio_set_dir(ULTRASONIC_ECHO_PIN, GPIO_IN); // Set the ECHO pin as an input

    // Configure interrupts to capture rising and falling edges of the ECHO pin
    // gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_pin_handler);

    gpio_put(ULTRASONIC_TRIG_PIN, false); // Set the ULTRASONIC_TRIG_PIN pin low initially
}

double getDistance() {

    double totalDistance = 0.0;
    
    for (int i = 0; i < NUM_READINGS; i++){
        totalDistance += durations_us[i];

    }

    if (totalDistance > 0.0) {
        return totalDistance / NUM_READINGS; // Calculate the average distance
    } else {
        return -1.0; // Return -1 for Measurement Timeout
    }
}