#ifndef IR_BARCODE_H
#define IR_BARCODE_H

#define IR_PIN 28              // Assign GPIO Pin 28 as the ADC Input
#define MAX_PULSE_READING_SIZE 200
#define MAX_OUTPUT_SIZE 10
#define TIMEOUT_MS 3000
#define HASH_TABLE_SIZE 44

void setup_barcode();
void barcode_scanning_interrupt(uint gpio, uint32_t events);
#endif
