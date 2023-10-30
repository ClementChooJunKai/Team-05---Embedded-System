#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define IR_SENSOR_PIN_LEFT 26 // GPIO 26 for the IR sensor pin on the right
#define IR_SENSOR_PIN_RIGHT 27 // GPIO 27 for the IR sensor pin on the right
#define DETECTION_THRESHOLD 500 // Average threshold of light detection. ADC readings above this threshold means that the sensor does not detect any light 

int main() {
    // Initialize necessary peripherals
    stdio_usb_init();
    adc_init();
    adc_gpio_init(IR_SENSOR_PIN_LEFT); // Initialize left IR sensor pin
    adc_select_input(0); // Select ADC input 0
    adc_gpio_init(IR_SENSOR_PIN_RIGHT); // Initialize right IR sensor pin
    adc_select_input(1); // Select ADC input 1
    
    gpio_set_dir(IR_SENSOR_PIN_LEFT, GPIO_IN); // Set GPIO direction
    gpio_set_dir(IR_SENSOR_PIN_RIGHT, GPIO_IN); // Set GPIO direction
    gpio_pull_down(IR_SENSOR_PIN_LEFT); // Set GPIO pin to be pulled down
    gpio_pull_down(IR_SENSOR_PIN_RIGHT); // Set GPIO pin to be pulled down

    while (1) {
        uint16_t leftResult = adc_read(); // Read the ADC value from the left IO sensor
        uint16_t rightResult = adc_read(); // Read the ADC value from the right IO sensor

        printf("Left sensor: %d, Right sensor: %d. ", leftResult, rightResult); // Print out the ADC value

        // Perform some action based on the ADC value
        if (leftResult > DETECTION_THRESHOLD && rightResult > DETECTION_THRESHOLD) { // if nothing is detected
            printf("Wall detected on both sensors. Turn back!\n"); // reverse car and turn it left or right randomly
            /
        } else if (leftResult > DETECTION_THRESHOLD) { // left sensor detects obstacle
            // main code
            printf("Wall detected on left sector. Turn right.\n");
        } else if (rightResult > DETECTION_THRESHOLD) { // right sensor detects obstacle
            // main code
            printf("Wall detected on right sector. Turn left.\n");
        } else {
            // main code
            printf("No wall detected. Continue moving forward.\n"); // move straight
        }
        sleep_ms(1000); // Adjust the delay as needed
    }

    return 0;
}
