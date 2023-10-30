#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

const uint TRIG_PIN = 22; // GPIO 22 for the TRIG pin
const uint ECHO_PIN = 20; // GPIO 20 for the ECHO pin

static absolute_time_t start_time;
static absolute_time_t end_time;
static bool rising_edge = true;
const int NUM_READINGS = 100; // Number of readings to average for better accuracy

void echo_pin_handler(uint gpio, uint32_t events) {
    if (rising_edge) {
        start_time = get_absolute_time(); // Record the time of a rising edge
        gpio_set_irq_enabled(ECHO_PIN, GPIO_IRQ_EDGE_FALL, true);
    } else {
        end_time = get_absolute_time(); // Record the time of a falling edge
        gpio_set_irq_enabled(ECHO_PIN, GPIO_IRQ_EDGE_RISE, true);
    }
    rising_edge = !rising_edge; // Toggle the edge detection state
}

void setupUltrasonic() {
    stdio_init_all(); // Initialize the standard I/O for printing
    gpio_init(TRIG_PIN); // Initialize the TRIG pin
    gpio_set_dir(TRIG_PIN, GPIO_OUT); // Set the TRIG pin as an output
    gpio_init(ECHO_PIN); // Initialize the ECHO pin
    gpio_set_dir(ECHO_PIN, GPIO_IN); // Set the ECHO pin as an input

    // Configure interrupts to capture rising and falling edges of the ECHO pin
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE, true, &echo_pin_handler);
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_FALL, true, &echo_pin_handler);

    gpio_put(TRIG_PIN, false); // Set the TRIG pin low initially
    sleep_ms(500); // Wait for 0.5 seconds
}

double getDistance() {
    double totalDistance = 0.0;

    for (int i = 0; i < NUM_READINGS; i++) {
        // Send a trigger pulse
        gpio_put(TRIG_PIN, true);
        sleep_us(10); // Keep it high for 10 microseconds
        gpio_put(TRIG_PIN, false); // Set the TRIG pin low

        // Wait for the measurement to complete (interrupts handle the timing)
        sleep_ms(10); // Reduce the timeout to 10 milliseconds

        // Calculate the distance based on the timestamps
        int32_t duration_us = absolute_time_diff_us(start_time, end_time);
        double distance = duration_us / 58.0f; // Convert to centimeters (approximate)

        if (distance > 0.0 && distance < 400.0) { // Check if the distance is within a valid range
            totalDistance += distance;
        }
    }

    if (totalDistance > 0.0) {
        return totalDistance / NUM_READINGS; // Calculate the average distance
    } else {
        return -1.0; // Return -1 for Measurement Timeout
    }
}

int main() {
    setupUltrasonic(); // Initialize the ultrasonic sensor
    
    while (1) {
        double distance = getDistance(); // Get the measured distance
        if (distance >= 0.0) {
            printf("Distance: %.2f cm\n", distance); // Print the distance
        } else {
            printf("Error: Measurement Timeout\n"); // Print an error message on timeout
        }
        sleep_ms(100); // Wait for 0.1 second before the next measurement
    }

    return 0;
}
