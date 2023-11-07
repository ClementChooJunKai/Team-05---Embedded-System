#ifndef MOTOR_H
#define MOTOR_H

// [MOTOR] Left Motor
#define IN1 10 
#define IN2 11
#define ENA 12

// [MOTOR] Right Motor
#define ENB 19
#define IN3 20
#define IN4 21

int motor_set_speed(float motor_speed);
int motor_initialize();
int motor_forward();
int motor_reverse();
int motor_forward_right();
int motor_reverse_right();
int motor_forward_left();
int motor_reverse_left();
int motor_stop();

#endif  // MOTOR_H
