#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "hardware/i2c.h"
#include "car_magnetnometer/car_magnetnometer.h"
#include "car_ultrasonic/car_ultrasonic.h"
#include "car_wheel_driver/car_wheel_driver.h"
#include "car_irline/car_irline_sensor.h"
#include "car_irline/car_ir_barcode.h"
#include "car_wheel_encoder/car_wheel_encoder.h"
#include "car_pid_controller/car_pid_controller.h"
#include "car_pid_controller/car_pid_controller.h"


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

#define TARGET_SPEED 30.0


#define TARGET_SPEED 30.0


const char WIFI_SSID[] = "Yeetus";
const char WIFI_PASSWORD[] = "182408092000";
float duty_cycle_left = 0.7;
float duty_cycle_right = 0.7;
bool obstacle_detected = false;
bool wall_detected = false;

struct SSI_CarData_Struct ssi_car_data;



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


void ultrasonic_driver_task(__unused void *params)
{
    setupUltrasonic(); // Initialize the ultrasonic sensor
    while (1)
    {
        double distance = getDistance();
        // printf("DISTANCE: %f\n", distance);
        if (distance > 0 && distance <= 20.0)
        {
            // sleep_ms(10);
            motor_stop();
            obstacle_detected = true;
            motor_reverse(duty_cycle_left, duty_cycle_right);
        }
        else
        {
            // Allow motor to continue reversing for a short while
            if (obstacle_detected == true)
            {
                vTaskDelay(50);
                motor_stop();
                obstacle_detected = false;
            }
        }
        vTaskDelay(100);
        ssi_car_data.ultrasonic_distance = distance;
    }
}

// TASK 3: Motor Driver (Includes wheel encoder)
void motor_driver_task(__unused void *params)
{
    setupWheelEncoder();
    motor_initialize();
    motor_forward(duty_cycle_left, duty_cycle_right);
    vTaskDelay(3000);

    while (1)
    {
        float current_left_speed = getLeftMotorSpeed();
        float current_right_speed = getRightMotorSpeed();
        printf("Left speed: %f, Right speed: %f", current_left_speed, current_right_speed);
        if (current_left_speed == 0 || current_right_speed == 0){
            duty_cycle_left = 1;
            duty_cycle_right = 1;
        }
        else
        {
            float target_speed = (current_left_speed + current_right_speed) / 2;
            float pid_out_left = pidUpdateLeft(target_speed, current_left_speed);
            float pid_out_right = pidUpdateRight(target_speed, current_right_speed);
            printf("Pid_L:%f Pid_R:%f", pid_out_left, pid_out_right);
            duty_cycle_left += pid_out_left;
            duty_cycle_right += pid_out_right;

            // Ensure duty cycle remains within valid range (0 to 1)
            if (duty_cycle_left > 1.0 || duty_cycle_left < 0.0) {
                resetLeftIntegral();
                duty_cycle_left = 1;
            } 
            if (duty_cycle_right > 1.0 || duty_cycle_right < 0.0) {
                resetRightIntegral();
                duty_cycle_right = 1;
            }
        }
        printf("L:%f, R:%f", duty_cycle_left, duty_cycle_right);

        if (!obstacle_detected && !wall_detected){
            motor_forward(duty_cycle_left, duty_cycle_right);
        }
        vTaskDelay(1000);
    }
}

void IR_driver_task(__unused void *params)
{
    ir_sensor_init();
    //setup_barcode();
    uint16_t leftResult, rightResult;
    xyz_struct magnetnometer_readings;
    int32_t compass_degree;

    while (1) // Infinite loop for continuous operation
    {
        vTaskDelay(50);
        if (!obstacle_detected){
            ir_sensor_read(&leftResult, &rightResult);

            if (leftResult > DETECTION_THRESHOLD && rightResult > DETECTION_THRESHOLD){
                wall_detected = true;
                motor_stop();
                vTaskDelay(100);
                get_magnetnometer_xyz_values(&magnetnometer_readings);
                compass_degree = convert_to_degrees(&magnetnometer_readings);
                printf("Magnetic North: %d degrees\n", compass_degree);
                motor_reverse_right(duty_cycle_left, duty_cycle_right);
                
                ir_sensor_read(&leftResult, &rightResult);
                if (leftResult > DETECTION_THRESHOLD && rightResult > DETECTION_THRESHOLD){
                    motor_forward_left(duty_cycle_left, duty_cycle_right);
                    vTaskDelay(350);
                    motor_reverse_left(duty_cycle_left, duty_cycle_right);
                    vTaskDelay(350);
                }
            }
            else if (leftResult > DETECTION_THRESHOLD)
            {
                wall_detected = true; // Set flag for wall detection
                motor_rotate_right(duty_cycle_left, duty_cycle_right); // Rotate right
            }
            else if (rightResult > DETECTION_THRESHOLD)
            {
                wall_detected = true; // Set flag for wall detection
                motor_rotate_left(duty_cycle_left, duty_cycle_right); // Rotate left
            }
            else{
                wall_detected = false;
                motor_forward(duty_cycle_left, duty_cycle_right);
            }
        }
        else
        {
            wall_detected = false; // Clear flag if obstacle detected elsewhere
        }
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
        printf("test");

        // use of SSI to contstantly retrieve new info & display onto web interface
        vTaskDelay(1000);

        // printf("CONNECTED\n");
    }

    cyw43_arch_deinit();
}

void interruptHandler(uint gpio, uint32_t events)
{
    if (gpio == ULTRASONIC_ECHO_PIN){
        ultrasonicHandler(events);
    }
    else if (gpio == LEFT_ENCODER_PIN){
        leftWheelEncoderHandler();
    }
    else if (gpio == RIGHT_ENCODER_PIN){
        rightWheelEncoderHandler();
    }
    else if (gpio == IR_PIN){
        //barcode_scanning_interrupt(gpio, events);
    }
}

void setupInterrupts()
{
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interruptHandler);
    gpio_set_irq_enabled(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true);
    //gpio_set_irq_enabled(IR_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

}

void vLaunch(void)
{
    // Declaring instances of our tasks
    // TaskHandle_t ledtask;
    TaskHandle_t wifi_taskTask;
    TaskHandle_t magnetnometer_driverTask;  
    TaskHandle_t ultrasonic_driverTask;
    TaskHandle_t motor_driverTask;
    TaskHandle_t ir_driverTask;

    xTaskCreate(wifi_task, "wifi_task_thread", configMINIMAL_STACK_SIZE, NULL, 2, &wifi_taskTask);
    xTaskCreate(magnetnometer_driver_task, "magnetnometer_driver_thread", configMINIMAL_STACK_SIZE, NULL, 4, &magnetnometer_driverTask);
    xTaskCreate(ultrasonic_driver_task, "ultrasonic_driver_thread", configMINIMAL_STACK_SIZE, NULL, 3, &ultrasonic_driverTask);
    xTaskCreate(motor_driver_task, "motor_driver_thread", configMINIMAL_STACK_SIZE, NULL, 2, &motor_driverTask);
    xTaskCreate(IR_driver_task, "irline_driver_thread", configMINIMAL_STACK_SIZE, NULL, 2, &ir_driverTask);

    setupInterrupts();
    

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
    stdio_usb_init();
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