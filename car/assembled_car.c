#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "hardware/i2c.h"
#include "car_magnetnometer/car_magnetnometer.h"
#include "car_ultrasonic/car_ultrasonic.h"
#include "car_wheel_driver/car_wheel_driver.h"
#include "car_irline/car_irline_sensor.h"


#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "pico/lwip_freertos.h"
#include "pico/lwip_nosys.h"
// #include "lwip/async.h"
#include "lwip/init.h"
#include "lwip/sys.h"
// #include "lwip/sockets.h"

#include "lwipopts.h"
#include "web_interface/ssi.h"
#include "web_interface/cgi.h"
#include "lwip/netif.h"

#define mbaTASK_MESSAGE_BUFFER_SIZE (60)
#define DATA_POINT_COUNTER 10 // This variable defines the no of data points we are using for moving temp averaging

#define TEST_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

const char WIFI_SSID[] = "Nicolas";
const char WIFI_PASSWORD[] = "11223344";

struct SSI_CarData_Struct ssi_car_data;

// TASK 1: magnetnometer Driver
void magnetnometer_driver_task(__unused void *params)
{
    init_i2c_and_sensors(); // Inititalizes I2C Port & setups sensors

    xyz_struct magnetnometer_readings, accelerometer_readings;
    xyz_struct_float accelerometer_readings_in_Gs;

    while (1)
    {
        // test_sensor_connection();
        get_magnetnometer_xyz_values(&magnetnometer_readings);
        int32_t compass_degree = convert_to_degrees(&magnetnometer_readings);
        // printf("X = %d, Y = %d, Z = %d\n", magnetnometer_readings.x, magnetnometer_readings.y, magnetnometer_readings.z);
        printf("Magnetic North: %d degrees\n", compass_degree);

        get_accelerometer_xyz_values(&accelerometer_readings);
        convert_accelerometer_to_Gs(&accelerometer_readings, &accelerometer_readings_in_Gs);

        // printf("\nData:\n");
        // printf("X = %d, Y = %d, Z = %d\n", accelerometer_readings.x, accelerometer_readings.y, accelerometer_readings.z);
        // printf("X = %.2f, Y = %.2f, Z = %.2f\n", accelerometer_readings_in_Gs.x, accelerometer_readings_in_Gs.y, accelerometer_readings_in_Gs.z);

        ssi_car_data.compass_degree = compass_degree;
        ssi_car_data.accelerometer_readings_in_Gs.x = accelerometer_readings_in_Gs.x;
        ssi_car_data.accelerometer_readings_in_Gs.y = accelerometer_readings_in_Gs.y;
        ssi_car_data.accelerometer_readings_in_Gs.z = accelerometer_readings_in_Gs.z;
        vTaskDelay(800);
    }
}

// TASK 2: Ultrasonic Driver
void ultrasonic_driver_task(__unused void *params)
{
    setupUltrasonic(); // Initialize the ultrasonic sensor

    while (1)
    {
        double distance = getDistance(); // Get the measured distance
        if (distance >= 0.0)
        {
            printf("Distance: %.2f cm\n", distance); // Print the distance
        }
        else
        {
            printf("Error: Measurement Timeout\n"); // Print an error message on timeout
        }
        // vTaskDelay(ULTRASONIC_SAMPLING_RATE); // Wait for 0.1 second before the next measurement
        ssi_car_data.ultrasonic_distance = distance;
        vTaskDelay(500); // Wait for 0.1 second before the next measurement
    }
}

// TASK 3: Motor Driver (Includes wheel encoder)
void motor_driver_task(__unused void *params)
{
    motor_initialize();
    sleep_ms(2500);
    while (1)
    {
        motor_set_speed(1);
        // motor_forward();
        vTaskDelay(500);
    }
}

void IR_driver_task(__unused void *params)
{
    ir_sensor_init();
    uint16_t leftResult, rightResult;

    while (1)
    {
        ir_sensor_read(&leftResult, &rightResult);
        vTaskDelay(500);
    }
}

void wifi_task(__unused void *params)
{

    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        exit(1);
    }
    else
    {
        printf("Connected.\n");
    }

    // Get and print the IP address
    struct netif *netif = netif_default; // Get the default network interface
    if (netif != NULL)
    {
        ip4_addr_t ipaddr = netif->ip_addr;
        printf("IP Address: %s\n", ip4addr_ntoa(&ipaddr));
    }

    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");

    // Configure SSI and CGI handler
    ssi_init();
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    while (true)
    {
        // use of SSI to contstantly retrieve new info & display onto web interface
        vTaskDelay(1000);

        // printf("CONNECTED\n");
    }

    cyw43_arch_deinit();
}

void vLaunch(void)
{
    // Declaring instances of our tasks
    TaskHandle_t wifi_taskTask;
    // TaskHandle_t ledtask;
    TaskHandle_t magnetnometer_driverTask;
    TaskHandle_t ultrasonic_driverTask;
    TaskHandle_t motor_driverTask;
    TaskHandle_t ir_driverTask;

    xTaskCreate(wifi_task, "wifi_task_thread", configMINIMAL_STACK_SIZE, NULL, TEST_TASK_PRIORITY, &wifi_taskTask);
    // xTaskCreate(led_task, "TestLedThread", configMINIMAL_STACK_SIZE, NULL, 5, &ledtask);
    xTaskCreate(magnetnometer_driver_task, "magnetnometer_driver_thread", configMINIMAL_STACK_SIZE, NULL, 4, &magnetnometer_driverTask);
    xTaskCreate(ultrasonic_driver_task, "ultrasonic_driver_thread", configMINIMAL_STACK_SIZE, NULL, 3, &ultrasonic_driverTask);
    xTaskCreate(motor_driver_task, "motor_driver_thread", configMINIMAL_STACK_SIZE, NULL, 2, &motor_driverTask);
    xTaskCreate(IR_driver_task, "irline_driver_thread", configMINIMAL_STACK_SIZE, NULL, 2, &ir_driverTask);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    // we must bind the main task to one core (well at least while the init is called)
    // (note we only do this in NO_SYS mode, because cyw43_arch_freertos
    // takes care of it otherwise)
    vTaskCoreAffinitySet(task, 1);
#endif

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
}

int main(void)
{
    stdio_init_all();
    sleep_ms(5000);
    /* Configure the hardware ready to run the demo. */
    const char *rtos_name;
#if (portSUPPORT_SMP == 1)
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if (portSUPPORT_SMP == 1) && (configNUM_CORES == 2)
    printf("Starting %s on both cores:\n", rtos_name);
    vLaunch();
#elif (RUN_FREERTOS_ON_CORE == 1)
    printf("Starting %s on core 1:\n", rtos_name);
    multicore_launch_core1(vLaunch);
    while (true)
        ;
#else
    printf("Starting %s on core 0:\n", rtos_name);
    vLaunch();
#endif
    return 0;
}