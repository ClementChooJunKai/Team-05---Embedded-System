// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "car_ir_barcode.h"

// Define arrays and variables for pulse durations, thresholds, and data storage
uint16_t pulse_durations[MAX_PULSE_READING_SIZE];
uint16_t temp_list[9];
char output[MAX_OUTPUT_SIZE];
int threshold;

// Define structures for the hash table
typedef struct KeyValuePair {
    char key[10];
    char value;
    struct KeyValuePair* next;
} KeyValuePair;

typedef struct {
    int size;
    KeyValuePair* table[HASH_TABLE_SIZE];
} HashTable;

HashTable code39Table;

// Computes a hash value based on the ASCII sum of the key
int hashFunction(const char* key) {
    // A simple hash function using the sum of ASCII values
    int hash = 0;
    for (int i = 0; i < strlen(key); ++i) {
        hash += key[i];
    }
    return hash % HASH_TABLE_SIZE;
}
// Initializes the hash table, setting all table entries to NULL
void initializeHashTable(HashTable* hashTable) {
    hashTable->size = HASH_TABLE_SIZE;
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        hashTable->table[i] = NULL;
    }
}
// Inserts a key-value pair into the hash table
void insert(HashTable* hashTable, const char* key, char value) {
    int index = hashFunction(key);
    KeyValuePair* newPair = (KeyValuePair*)malloc(sizeof(KeyValuePair));
    strncpy(newPair->key, key, sizeof(newPair->key) - 1);  // Copy key, ensuring null-termination
    newPair->key[sizeof(newPair->key) - 1] = '\0';  // Null-terminate the key
    newPair->value = value;
    newPair->next = hashTable->table[index];
    hashTable->table[index] = newPair;
}
// Searches for a key in the hash table and returns the corresponding value
char search(HashTable* hashTable, const char* key) {
    int index = hashFunction(key);
    KeyValuePair* current = hashTable->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return '?';  // Key not found
}
// Initializes the code39Table with predefined key-value pairs
void setup_hash_table(){
    
    initializeHashTable(&code39Table);

    insert(&code39Table, "000110100\0", '0');
    insert(&code39Table, "100100001\0", '1');
    insert(&code39Table, "001100001\0", '2');
    insert(&code39Table, "101100000\0", '3');
    insert(&code39Table, "000110001\0", '4');
    insert(&code39Table, "100110000\0", '5');
    insert(&code39Table, "001110000\0", '6');
    insert(&code39Table, "000100101\0", '7');
    insert(&code39Table, "100100100\0", '8');
    insert(&code39Table, "001100100\0", '9');


    insert(&code39Table, "100001001\0", 'A');
    insert(&code39Table, "001001001\0", 'B');
    insert(&code39Table, "101001000\0", 'C');
    insert(&code39Table, "000011001\0", 'D');
    insert(&code39Table, "100011000\0", 'E');
    insert(&code39Table, "001011000\0", 'F');
    insert(&code39Table, "000001101\0", 'G');
    insert(&code39Table, "100001100\0", 'H');
    insert(&code39Table, "001001100\0", 'I');
    insert(&code39Table, "000011100\0", 'J');
    insert(&code39Table, "100000011\0", 'K');
    insert(&code39Table, "001000011\0", 'L');
    insert(&code39Table, "101000010\0", 'M');
    insert(&code39Table, "000010011\0", 'N');
    insert(&code39Table, "100010010\0", 'O');
    insert(&code39Table, "001010010\0", 'P');
    insert(&code39Table, "000000111\0", 'Q');
    insert(&code39Table, "100000110\0", 'R');
    insert(&code39Table, "001000110\0", 'S');
    insert(&code39Table, "000010110\0", 'T');
    insert(&code39Table, "110000001\0", 'U');
    insert(&code39Table, "011000001\0", 'V');
    insert(&code39Table, "111000000\0", 'W');
    insert(&code39Table, "010010001\0", 'X');
    insert(&code39Table, "110010000\0", 'Y');
    insert(&code39Table, "011010000\0", 'Z');

    insert(&code39Table, "010000101\0", '-');
    insert(&code39Table, "110000100\0", '.');
    insert(&code39Table, "011000100\0", ' ');
    insert(&code39Table, "010101000\0", '$');
    insert(&code39Table, "010100010\0", '/');
    insert(&code39Table, "010001010\0", '+');
    insert(&code39Table, "000101010\0", '%');
    insert(&code39Table, "010010100\0", '*');
}
// Initializes ADC for IR sensor readings
void setup_adc(){
    adc_init();
    adc_gpio_init(IR_PIN);
    gpio_set_dir(IR_PIN, GPIO_IN);
    adc_set_temp_sensor_enabled(false);
    adc_select_input(2);
 }
// Resets all elements in the pulse_durations array to zero
 void reset_pulse_durations(){
    for (int i = 0; i < MAX_PULSE_READING_SIZE; i++){
        pulse_durations[i] = 0;
    }
 }
// Sorts an array using insertion sort algorithm
void insertion_sort(uint16_t arr[], uint16_t n) {
    uint16_t i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            // Swap arr[j] and arr[j + 1]
            uint16_t temp = arr[j];
            arr[j] = arr[j + 1];
            arr[j + 1] = temp;

            j = j - 1;
        }
    }
}
// Checks for the presence of a start character (*) in the barcode
 bool check_for_asterisk(int end_index){
    int start_index = end_index - 8;
    char reading[10] = "000000000\0";

    for (int i = start_index; i <= end_index; i++){
        temp_list[i-start_index] = pulse_durations[i];
    }

    // Find avg of 3 smallest values and multiply by 2 to get threshold
    insertion_sort(temp_list, 9);
    threshold = ((temp_list[0] + temp_list[1] + temp_list[2]) / 3) * 2;
    
    for (int i = start_index; i <= end_index; i++){
        if (pulse_durations[i] > threshold){
            reading[i - start_index] = '1';
        }
    }

    if (search(&code39Table, reading) == '*'){
        printf("\nCODE 39 START CHAR (*) DETECTED\n");
        return true;
    }
    else{
        return false;
    }
 }
// Identifies a character based on pulse durations and hash table lookup
char identify_character(int end_index){
    char search_key[10] = "000000000\0";
    int start_index = end_index - 8;
    for (int i = start_index; i <= end_index; i++){
        if (pulse_durations[i] > threshold){
            search_key[i - start_index] = '1';
        }
    }
    return search(&code39Table, search_key);
 }
// Adds a character to a string if it doesn't exceed the maximum size
bool add_char_to_string(char* str, char c, int max_size){
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
// Interrupt handler for barcode scanning

void barcode_scanning_interrupt(uint gpio, uint32_t events) {
    // Maintain the index of the current pulse duration and the time of the previous pulse
    static int pulse_duration_index = 0;
    static uint32_t previous_time = 0;

    if (gpio != IR_PIN){
        return;
    }

    // Get the current time in microseconds
    uint32_t current_time = time_us_32();

    // Calculate the duration of the current pulse in microseconds and milliseconds
    uint32_t pulse_duration_us = current_time - previous_time;
    uint32_t pulse_duration_ms = pulse_duration_us / 1000;
    if (pulse_duration_ms < 1){
        return;
    }
    previous_time = current_time;

    // Maintain the scanning state and the index of the next expected character
    static bool scanning = false;
    static int next_char_index;

    // Print the duration of the current pulse in milliseconds
    printf("%d ms\n", pulse_duration_ms);

    // Check if the pulse duration is within the timeout range
    if (pulse_duration_ms < TIMEOUT_MS) {
        pulse_durations[pulse_duration_index] = pulse_duration_ms;
    } else {
        // Reset variables and scanning state if timeout exceeds
        reset_pulse_durations();
        pulse_duration_index = 0;
        next_char_index = 0;
        scanning = false;
        return;
    }

    // Check if the pulse duration index exceeds the maximum size or reading limit
    if ((pulse_duration_index > MAX_PULSE_READING_SIZE) || (next_char_index > MAX_PULSE_READING_SIZE)) {
        printf("\nReading limit exceeded. Resetting..\n");
        reset_pulse_durations();
        pulse_duration_index = 0;
        next_char_index = 0;
        scanning = false;
        return;
    }

    // Check for the start of a barcode sequence and initiate scanning if found
    if (pulse_duration_index >= 8 && !scanning) {
        scanning = check_for_asterisk(pulse_duration_index);
        if (scanning) {
            output[0] = '\0';
            next_char_index = pulse_duration_index + 10;
        }
    }

    // Process scanned characters if scanning is active and the expected index is reached
    if (scanning && (pulse_duration_index == next_char_index)) {
        char scanned_char = identify_character(pulse_duration_index);
        if (pulse_durations[pulse_duration_index - 9] > threshold || scanned_char == '?') {
            printf("\nUnexpected value detected. Resetting...\n");
            reset_pulse_durations();
            pulse_duration_index = 0;
            next_char_index = 0;
            scanning = false;
            return;
        } else {
            if (scanned_char == '*') {
                printf("\nEnd character detected. Output: ");
                printf(output);
                printf("\n");
                reset_pulse_durations();
                pulse_duration_index = 0;
                next_char_index = 0;
                scanning = false;
                return;
            } else {
                // Add the scanned character to the output string if space available
                if (add_char_to_string(output, scanned_char, MAX_OUTPUT_SIZE)) {
                    next_char_index += 10;
                } else {
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
    // Move to the next pulse duration index
    pulse_duration_index++;
}
// Sets up necessary components for barcode scanning
void setup_barcode() {
    setup_adc();
    setup_hash_table();
 }
