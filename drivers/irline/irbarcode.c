#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define IR_PIN 26              // Assign GPIO Pin 26 as the ADC Input
#define MAX_PULSE_READING_SIZE 200
#define MAX_OUTPUT_SIZE 10
#define TIMEOUT_MS 3000
#define HASH_TABLE_SIZE 44

uint16_t pulse_durations[MAX_PULSE_READING_SIZE];
char output[MAX_OUTPUT_SIZE];
int threshold;

/*void setup_hash_table(){
    insert_hash_table("000110100\0", '0');
    insert_hash_table("100001001\0", 'A');
    insert_hash_table("010010100\0", '*');
}*/

void setup_adc(){
    adc_init();
    adc_gpio_init(IR_PIN);
    adc_set_temp_sensor_enabled(false);
    adc_select_input(0);
 }

 void reset_pulse_durations(){
    for (int i = 0; i < MAX_PULSE_READING_SIZE; i++){
        pulse_durations[i] = 0;
    }
 }

 bool check_for_asterisk(int end_index){
    int start_index = end_index - 8;
    char asterisk_code[10] = "010010100\0"; // Asterisk *
    char reading[10] = "000000000\0";

    int smallest_reading = pulse_durations[start_index];

    for (int i = start_index + 1; i <= end_index; i++){
        if (smallest_reading > pulse_durations[i]){
            smallest_reading = pulse_durations[i];
        }
    }

    threshold = smallest_reading * 2.2;

    for (int i = start_index; i <= end_index; i++){
        if (pulse_durations[i] > threshold){
            reading[i - start_index] = '1';
        }
    }

    if (strcmp(reading, asterisk_code) == 0){
        printf("\nCODE 39 START CHAR (*) DETECTED\n");
        return true;
    }
    else{
        return false;
    }
 }

/*char identify_character(int end_index){
    char search_key[10] = "000000000\0";
    int start_index = end_index - 8;
    for (int i = start_index; i <= end_index; i++){
        if (pulse_durations[i] > threshold){
            search_key[i - start_index] = '1';
        }
    }
    return search_hash_table(search_key);
 }*/

bool addCharToString(char* str, char c, int max_size){
    int len = strlen(str);
    if (len < max_size - 1){
        str[len] = c;
        str[len + 1] = '\0';
        return true;
    }
    else{
        return false;
    }
 }

void barcode_scanning_interrupt(uint gpio, uint32_t events) {

    static int pulse_duration_index = 0;
    static uint32_t previous_time = 0;

    uint32_t current_time = time_us_32();

    uint32_t pulse_duration_us = current_time - previous_time;
    uint32_t pulse_duration_ms = pulse_duration_us / 1000;
    previous_time = current_time;

    static bool scanning = false;
    static int next_char_index;

    printf("%d ms\n", pulse_duration_ms);


    if (pulse_duration_ms < TIMEOUT_MS){
        pulse_durations[pulse_duration_index] = pulse_duration_ms;
    } 
    else{
        reset_pulse_durations();
        pulse_duration_index = 0;
        next_char_index = 0;
        scanning = false;
        return;
    }

    
    if ((pulse_duration_index > MAX_PULSE_READING_SIZE) || (next_char_index > MAX_PULSE_READING_SIZE)){
        printf("\nReading limit exceeded. Resetting..\n");
        reset_pulse_durations();
        pulse_duration_index = 0;
        next_char_index = 0;
        scanning = false;
        return;
    }


    if (pulse_duration_index >= 8 && !scanning){
        scanning = check_for_asterisk(pulse_duration_index);
        if (scanning){
            output[0] = '\0';
            next_char_index = pulse_duration_index + 10;
        }
    }
    if (scanning && (pulse_duration_index == next_char_index)){
        if (pulse_durations[pulse_duration_index - 9] > threshold){
            printf("\nUnexpected value detected. Resetting...\n");
            reset_pulse_durations();
            pulse_duration_index = 0;
            next_char_index = 0;
            scanning = false;
            return;
        }
        else{
            //char scanned_char = identify_character(pulse_duration_index);
            char scanned_char = '*';
            if (scanned_char == '*'){
                printf("\nEnd character detected. Output: ");
                printf(output);
                printf("\n");
                reset_pulse_durations();
                pulse_duration_index = 0;
                next_char_index = 0;
                scanning = false;
                return;
            }
            else{
                if (addCharToString(output, scanned_char, MAX_OUTPUT_SIZE)){
                    next_char_index += 10;
                }
                else{
                    printf("\nMax output exceeded, resetting.\n");
                    reset_pulse_durations();
                    pulse_duration_index = 0;
                    next_char_index = 0;
                    scanning = false;
                    return;
                }
            }
        }
    }
    pulse_duration_index++;
}

void setup() {
    stdio_usb_init();
    setup_adc();
    gpio_init(IR_PIN);
    gpio_set_dir(IR_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(IR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &barcode_scanning_interrupt);
 }

int main() {
    // Initialize necessary peripherals
    setup();

    while (1) { // Main code

    }

    return 0;
}
