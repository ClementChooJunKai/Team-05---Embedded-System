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

void motor_set_speed(float left_motor_speed, float right_motor_speed);
int motor_initialize();
int motor_forward(float left_motor_speed, float right_motor_speed);
int motor_reverse(float left_motor_speed, float right_motor_speed);
int motor_forward_right(float left_motor_speed, float right_motor_speed);
int motor_reverse_right(float left_motor_speed, float right_motor_speed);
int motor_forward_left(float left_motor_speed, float right_motor_speed);
int motor_reverse_left(float left_motor_speed, float right_motor_speed);
int motor_stop();

#endif  // MOTOR_H
