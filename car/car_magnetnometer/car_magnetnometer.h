#ifndef CAR_MAGNETNOMETER_H
#define CAR_MAGNETNOMETER_H

#define SDA_PIN 4
#define SCL_PIN 5
#define I2C_PORT i2c0
#define MAGNET_SENSOR_ADDR 0x1E
#define ACCELEROMETER_SENSOR_ADDRESS 0x19
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Z_H_M 0x05
#define OUT_Z_L_M 0x06
#define OUT_Y_H_M 0x07
#define OUT_Y_L_M 0x08
#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D
#define ACCELEROMETER_XYZ_REGISTER_ADDR 0xA8
#define EARTH_GRAVITY 9.80665
#define ACCELEROMETER_SENSITIVITY 0.001

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} xyz_struct;

typedef struct
{
    float x;
    float y;
    float z;
} xyz_struct_float;

void test_sensor_connection();
int get_magnetnometer_xyz_values(xyz_struct *magnetnometer_readings);
int32_t convert_to_degrees(xyz_struct *magnetnometer_readings);
int convert_accelerometer_to_Gs(xyz_struct *accelerometer_readings, xyz_struct_float *accelerometer_readings_in_Gs);
int get_accelerometer_xyz_values(xyz_struct *accelerometer_readings);
int init_i2c_and_sensors();

#endif
