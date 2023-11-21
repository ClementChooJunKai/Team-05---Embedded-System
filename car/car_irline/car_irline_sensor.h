#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#define IR_SENSOR_PIN_LEFT 26
#define IR_SENSOR_PIN_RIGHT 27
#define DETECTION_THRESHOLD 500

void ir_sensor_init();
void ir_sensor_read(uint16_t *leftResult, uint16_t *rightResult);
bool ir_read_left();
bool ir_read_right();
void ir_write_left(uint32_t events);
void ir_write_right(uint32_t events);

#endif
