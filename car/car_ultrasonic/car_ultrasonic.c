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


void ultrasonicHandler(uint32_t events)
{
    if (events == GPIO_IRQ_EDGE_RISE) 
    {
        start_time = get_absolute_time(); // Record the time of a rising edge
    } 
    else if (events == GPIO_IRQ_EDGE_FALL) 
    {
        end_time = get_absolute_time(); // Record the time of a falling edge
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
    // gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &ultrasonicHandler);

    gpio_put(ULTRASONIC_TRIG_PIN, false); // Set the ULTRASONIC_TRIG_PIN pin low initially
}

double getDistance() {

    double totalDistance = 0.0;
    for (int i = 0; i < NUM_READINGS; i++) {
        // Send a trigger pulse
        gpio_put(ULTRASONIC_TRIG_PIN, true);
        sleep_us(10); // Keep it high for 10 microseconds
        gpio_put(ULTRASONIC_TRIG_PIN, false); // Set the TRIG pin low
        
        // Calculate the distance based on the timestamps
        int32_t duration_us = absolute_time_diff_us(start_time, end_time);
        double distance = duration_us / 58.0f; // Convert to centimeters (approximate)
        if (distance > 0.0 && distance < 400.0) { // Check if the distance is within a valid range
            totalDistance += distance;
        }
    }
    if (totalDistance > 0.0) {
        return totalDistance/NUM_READINGS;
    } 
    else {
        return -1.0; // Return -1 for Measurement Timeout
    }
}