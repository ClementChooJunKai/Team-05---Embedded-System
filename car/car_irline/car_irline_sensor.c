#include <stdio.h>                  // Standard I/O library
#include "pico/stdlib.h"            // Pico standard library
#include "hardware/adc.h"           // Hardware ADC library
#include "car_irline_sensor.h"      // Include the header file

bool wall_left = false;             // Initialize left wall detection flag
bool wall_right = false;            // Initialize right wall detection flag
 
 // Initialize the IR sensors
void ir_sensor_init()
{
   
    adc_init();                   
    adc_gpio_init(IR_SENSOR_PIN_LEFT);  // Initialize left IR sensor pin
    adc_gpio_init(IR_SENSOR_PIN_RIGHT); // Initialize right IR sensor pin

    // Configure GPIO direction and pull-down resistors for the IR sensors
    gpio_set_dir(IR_SENSOR_PIN_LEFT, GPIO_IN);
    gpio_set_dir(IR_SENSOR_PIN_RIGHT, GPIO_IN);
    gpio_pull_down(IR_SENSOR_PIN_LEFT);
    gpio_pull_down(IR_SENSOR_PIN_RIGHT);
}

void ir_sensor_read(uint16_t *leftResult, uint16_t *rightResult)
{
    adc_select_input(0);            // Select ADC input 0 for left sensor
    *leftResult = adc_read();       // Read ADC value for left sensor
    
    adc_select_input(1);            // Select ADC input 1 for right sensor
    *rightResult = adc_read();      // Read ADC value for right sensor

    // Print out the ADC values (commented out)
    // printf("Left sensor: %d, Right sensor: %d. ", *leftResult, *rightResult);

    // Perform actions based on the ADC values (code commented out)
    /*if (*leftResult > DETECTION_THRESHOLD && *rightResult > DETECTION_THRESHOLD)
    {
        printf("Wall detected on both sensors. Turn back!\n");
    }
    else if (*leftResult > DETECTION_THRESHOLD)
    {
        printf("Wall detected on left sector. Turn right.\n");
    }
    else if (*rightResult > DETECTION_THRESHOLD)
    {
        printf("Wall detected on right sector. Turn left.\n");
    }
    else
    {
        printf("No wall detected. Continue moving forward.\n");
    }*/
}

// Callback function for left IR sensor events
void ir_write_left(uint32_t events){
    if (events == GPIO_IRQ_EDGE_RISE){
        wall_left = false;      // Clear left wall detection flag on rising edge
    }
    else{
        wall_left = true;       // Set left wall detection flag on falling edge
    }
}

// Callback function for right IR sensor events
void ir_write_right(uint32_t events){
    if (events == GPIO_IRQ_EDGE_RISE){
        wall_right = false;     // Clear right wall detection flag on rising edge
    }
    else{
        wall_right = true;      // Set right wall detection flag on falling edge
    }
}

// Function to read left wall detection status
bool ir_read_left(){
    return wall_left;           // Return the status of left wall detection flag
}

// Function to read right wall detection status
bool ir_read_right(){
    return wall_right;          // Return the status of right wall detection flag
}
