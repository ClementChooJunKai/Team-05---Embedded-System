#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#define IR_SENSOR_PIN_LEFT 26
#define IR_SENSOR_PIN_RIGHT 27
#define DETECTION_THRESHOLD 1000

void ir_sensor_init();
void ir_sensor_read(uint16_t *leftResult, uint16_t *rightResult);

#endif
