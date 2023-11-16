#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "car_irline_sensor.h" // Include the header file

void ir_sensor_init()
{
    // stdio_usb_init();
    adc_init();
    adc_gpio_init(IR_SENSOR_PIN_LEFT);  // Initialize left IR sensor pin
    adc_gpio_init(IR_SENSOR_PIN_RIGHT); // Initialize right IR sensor pin

    gpio_set_dir(IR_SENSOR_PIN_LEFT, GPIO_IN);
    gpio_set_dir(IR_SENSOR_PIN_RIGHT, GPIO_IN);
    gpio_pull_down(IR_SENSOR_PIN_LEFT);
    gpio_pull_down(IR_SENSOR_PIN_RIGHT);
}

void ir_sensor_read(uint16_t *leftResult, uint16_t *rightResult)
{
    adc_select_input(0);                // Select ADC input 0
    *leftResult = adc_read();
    
    adc_select_input(1);                // Select ADC input 1
    *rightResult = adc_read();

    printf("Left sensor: %d, Right sensor: %d. ", *leftResult, *rightResult); // Print out the ADC value

    // Perform some action based on the ADC value
    if (*leftResult > DETECTION_THRESHOLD && *rightResult > DETECTION_THRESHOLD)
    {                                                          // if nothing is detected
        printf("Wall detected on both sensors. Turn back!\n"); // reverse car and turn it left or right randomly
    }
    else if (*leftResult > DETECTION_THRESHOLD)
    { // left sensor detects obstacle
        // main code
        printf("Wall detected on left sector. Turn right.\n");
    }
    else if (*rightResult > DETECTION_THRESHOLD)
    { // right sensor detects obstacle
        // main code
        printf("Wall detected on right sector. Turn left.\n");
    }
    else
    {
        // main code
        printf("No wall detected. Continue moving forward.\n"); // move straight
    }
    sleep_ms(1000); // Adjust the delay as needed
}
