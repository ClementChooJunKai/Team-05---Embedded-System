// Datasheet of the GY-511 LSM303DLHC can be found in the following link:
// https://www.st.com/resource/en/datasheet/lsm303dlhc.pdf

#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <stdint.h>

#define SDA_PIN 4     // GP4 maps to I2C0's SDA
#define SCL_PIN 5     // GP5 maps to I2C0's SCL
#define I2C_PORT i2c0 // GP4 & GP5 of pico is mapped to I2C0 according to pico pin diagram

#define MAGNET_SENSOR_ADDR 0x1E           // Address of magnetic sensor according to datasheet
#define ACCELEROMETER_SENSOR_ADDRESS 0x19 // Address of accelrometer sensor according to datasheet

// Register addresses for magnetic sensor data
#define OUT_X_H_M 0x03 // High X
#define OUT_X_L_M 0x04 // Low X
#define OUT_Z_H_M 0x05 // High Z
#define OUT_Z_L_M 0x06 // Low Z
#define OUT_Y_H_M 0x07 // High Y
#define OUT_Y_L_M 0x08 // Low Y

// Register addresses for accelerometer sensor data
#define OUT_X_L_A 0x28 // Low X
#define OUT_X_H_A 0x29 // High X
#define OUT_Y_L_A 0x2A // Low Y
#define OUT_Y_H_A 0x2B // High Y
#define OUT_Z_L_A 0x2C // Low Z
#define OUT_Z_H_A 0x2D // High Z

#define ACCELEROMETER_XYZ_REGISTER_ADDR 0xA8

#define EARTH_GRAVITY 9.80665 // Earth's Gravity http://www.geophys.ac.cn/infowin/Gravity.asp#:~:text=The%20precise%20strength%20of%20Earth's,about%2032.1740%20ft%2Fs2).
#define ACCELEROMETER_SENSITIVITY 0.001 // Sensitivity of accelerometer is set to default with bits 00 whichi is +/-2g which corresponds to 1mg/LSB (page 10 of sensor datasheet). sensitivity = 1/1000

// Defined Structs to allow us to store data
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} xyz_struct; // Struct that magenometer & accelerometer sensor data can be stored as.

typedef struct
{
    float x;
    float y;
    float z;
} xyz_struct_float;

// Test I2C Connection with Magnetnometer Sensor. CHIP ID was not provided in datasheet hence used a selected/specified address.
void test_sensor_connection()
{
    sleep_ms(1000);

    // Read IRA_REG_M (0x0A) register on LSM303DLHC, expects default value of 0x48
    uint8_t check_reg = 0x0A; // IRA_REG_M register address
    uint8_t returned_addr;    // Used to store returned values after reading the register.

    i2c_write_blocking(I2C_PORT, MAGNET_SENSOR_ADDR, &check_reg, 1, true);     // Perform write to register 0x0A
    i2c_read_blocking(I2C_PORT, MAGNET_SENSOR_ADDR, &returned_addr, 1, false); // Reads its returned value from the register 0x0A which should be default value of 0x00 according to datasheet.

    if (returned_addr != 0x00)
    {
        printf("Wrong, current returned address: address returned = 0x%02X\n", returned_addr);
    }
    else
    {
        printf("CORRECT CHIP: address returned = 0x%02X\n", returned_addr);
    }
}

// Function used for I2C communication between pico & sensor module to retrieve xyz of Magnetnometer data
int get_magnetnometer_xyz_values(xyz_struct *magnetnometer_readings)
{
    uint8_t magnetnometer_xyz_registers_addr[6] = {OUT_X_H_M, OUT_X_L_M, OUT_Y_H_M, OUT_Y_L_M, OUT_Z_H_M, OUT_Z_L_M}; // Contains all the addresses where we are reading x,y,z values from. High & Low values included.
    uint8_t magnetnometer_xyz_raw_buffer[6];                                                                          // Contains all the raw uint8_t pulled from sensor.

    // Signal slave to read/write the addresses found in xyz_register_addr
    i2c_write_blocking(I2C_PORT, MAGNET_SENSOR_ADDR, magnetnometer_xyz_registers_addr, 6, true);
    i2c_read_blocking(I2C_PORT, MAGNET_SENSOR_ADDR, magnetnometer_xyz_raw_buffer, 6, false);

    // Optimize to store & convert (shift left 8 bits to become 16 bits) on the same line instead of using loops (loop unrolling)
    magnetnometer_readings->x = ((magnetnometer_xyz_raw_buffer[0] << 8) | (magnetnometer_xyz_raw_buffer[1])); // [0] Contains High X, [1] Contains Low X
    magnetnometer_readings->y = ((magnetnometer_xyz_raw_buffer[2] << 8) | (magnetnometer_xyz_raw_buffer[3])); // [2] Contains High Y, [3] Contains Low Y
    magnetnometer_readings->z = ((magnetnometer_xyz_raw_buffer[4] << 8) | (magnetnometer_xyz_raw_buffer[5])); // [4] Contains High Z, [5] Contains Low Z

    return 0;
}

// Function to convert xyz values into something more valueable for our project, degrees to north.
int32_t convert_to_degrees(xyz_struct *magnetnometer_readings)
{
    // Calculate the angle using the arctangent function with (x, y) values
    double angle_rad = atan2(magnetnometer_readings->x, magnetnometer_readings->y);

    // Convert radians to degrees
    double angle_deg = (angle_rad * (180.0 / M_PI));

    // Ensure the angle is within 0-360 degrees, similar to a compass
    if (angle_deg < 0)
    {
        angle_deg += 360;
    }

    // Type cast and return the angle degree.
    return (int32_t)angle_deg;
}

int convert_accelerometer_to_Gs(xyz_struct *accelerometer_readings, xyz_struct_float *accelerometer_readings_in_Gs)
{
    float sensitivity = ACCELEROMETER_SENSITIVITY;

    // Apply sensitivity scale factors to each axis
    accelerometer_readings_in_Gs->x = (float)accelerometer_readings->x * sensitivity;
    accelerometer_readings_in_Gs->y = (float)accelerometer_readings->y * sensitivity;
    accelerometer_readings_in_Gs->z = (float)accelerometer_readings->z * sensitivity;

    return 0;
}

// Function used for I2C communication between pico & sensor module to retrieve xyz of Accelerometer data
int get_accelerometer_xyz_values(xyz_struct *accelerometer_readings)
{

    // uint8_t accelerometer_xyz_registers_addr[6] = {OUT_X_L_A, OUT_X_H_A, OUT_Y_L_A, OUT_Y_H_A, OUT_Z_L_A, OUT_Z_H_A}; // Contains all the addresses where we are reading x,y,z accelerometer values from. High & Low values included.
    uint8_t accelerometer_xyz_registers_addr = ACCELEROMETER_XYZ_REGISTER_ADDR;
    uint8_t accelerometer_xyz_raw_buffer[6]; // Contains all the raw accelerometer data uint8_t pulled from sensor.

    // Signal slave to read/write the addresses found in accelerometer_xyz_registers_addr
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_SENSOR_ADDRESS, &accelerometer_xyz_registers_addr, 1, true);
    i2c_read_blocking(I2C_PORT, ACCELEROMETER_SENSOR_ADDRESS, accelerometer_xyz_raw_buffer, 6, false);

    // Optimize to store & convert (shift left 8 bits to become 16 bits) on the same line instead of using loops (loop unrolling)
    accelerometer_readings->x = ((accelerometer_xyz_raw_buffer[0]) | (accelerometer_xyz_raw_buffer[1] << 8)); // [0] Contains Low X, [1] Contains High X
    accelerometer_readings->y = ((accelerometer_xyz_raw_buffer[2]) | (accelerometer_xyz_raw_buffer[3] << 8)); // [2] Contains Low Y, [3] Contains High Y
    accelerometer_readings->z = ((accelerometer_xyz_raw_buffer[4]) | (accelerometer_xyz_raw_buffer[5] << 8)); // [4] Contains Low Z, [5] Contains High Z

    return 0;
}

// Inititalizes I2C Port & setups magenometer & accelerometer sensors.
int init_i2c_and_sensors()
{
    // Configure I2C Communication
    i2c_init(I2C_PORT, 400000); // Note that the controller does NOT support High speed mode or Ultra-fast speed mode, the fastest operation being fast mode plus at up to 1000Kb/s.
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    uint8_t config_data1[] = {0x02, 0x00}; // Configuration: Setting config to continuous-conversion mode.
    uint8_t config_data2[] = {0x01, 0xE0}; // Configuration: Gain settings config (1110000) Refer to page 38 of documentation
    uint8_t config_data3[] = {0x20, 0x77}; // Configuration: Sets data rate to normal mode at 400Hz, enable z,y,z axis.
    uint8_t config_data4[] = {0x23, 0x00}; // Configuration: Sets accelerometer to provide continuous data update
    i2c_write_blocking(I2C_PORT, MAGNET_SENSOR_ADDR, config_data1, sizeof(config_data1), false);
    i2c_write_blocking(I2C_PORT, MAGNET_SENSOR_ADDR, config_data2, sizeof(config_data2), false);
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_SENSOR_ADDRESS, config_data3, sizeof(config_data3), false);
    i2c_write_blocking(I2C_PORT, ACCELEROMETER_SENSOR_ADDRESS, config_data4, sizeof(config_data4), false);

    return 0;
}

// int main()
// {
//     stdio_init_all();       // Initilializes stdio for printing onto serial monitor.
//     init_i2c_and_sensors(); // Inititalizes I2C Port & setups sensors

//     xyz_struct magnetnometer_readings, accelerometer_readings;
//     xyz_struct_float accelerometer_readings_in_Gs;

//     while (1)
//     {
//         // test_sensor_connection();
//         // get_magnetnometer_xyz_values(&magnetnometer_readings);
//         // int32_t compass_degree = convert_to_degrees(&magnetnometer_readings);
//         // printf("X = %d, Y = %d, Z = %d\n", magnetnometer_readings.x, magnetnometer_readings.y, magnetnometer_readings.z);
//         // printf("Magnetic North: %d degrees\n", compass_degree);

//         get_accelerometer_xyz_values(&accelerometer_readings);
//         convert_accelerometer_to_Gs(&accelerometer_readings,&accelerometer_readings_in_Gs);
        
//         printf("\nData:\n");
//         printf("X = %d, Y = %d, Z = %d\n", accelerometer_readings.x, accelerometer_readings.y, accelerometer_readings.z);
//         printf("X = %.2f, Y = %.2f, Z = %.2f\n", accelerometer_readings_in_Gs.x, accelerometer_readings_in_Gs.y, accelerometer_readings_in_Gs.z);

//         sleep_ms(800);
//     }
// }
