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
#include "car_wheel_encoder/car_wheel_encoder.h"
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


const char WIFI_SSID[] = "Yeetus";
const char WIFI_PASSWORD[] = "182408092000";
float duty_cycle_left = 1;
float duty_cycle_right = 1;
bool obstacle_detected = false;
bool wall_detected = false;

struct SSI_CarData_Struct ssi_car_data;


// TASK 3: Motor Driver (Includes wheel encoder)
void motor_driver_task(__unused void *params)
{
    setupWheelEncoder(); // Set up wheel encoders
    motor_initialize(); // Initialize the motor
    motor_forward(duty_cycle_left, duty_cycle_right); // Move the motor forward initially
    vTaskDelay(2000); // Wait for 2 seconds before starting the main loop

    while (1) // Continuous operation
    {
        // Get current speed readings from wheel encoders
        float current_left_speed = getLeftMotorSpeed();
        float current_right_speed = getRightMotorSpeed();
        printf("Left speed: %f, Right speed: %f", current_left_speed, current_right_speed);

        // If either of the speeds is zero, set duty cycles to default
        if (current_left_speed == 0 || current_right_speed == 0){
            duty_cycle_left = 1;
            duty_cycle_right = 1;
        }
        else
        {
            // Calculate the target speed as the average of current speeds
            float target_speed = (current_left_speed + current_right_speed) / 2;

            // Compute PID outputs for left and right motors based on target and current speeds
            float pid_out_left = pidUpdateLeft(target_speed, current_left_speed);
            float pid_out_right = pidUpdateRight(target_speed, current_right_speed);
            printf("Pid_L:%f Pid_R:%f", pid_out_left, pid_out_right);

            // Update duty cycles based on PID outputs
            duty_cycle_left += pid_out_left;
            duty_cycle_right += pid_out_right;

            // Ensure duty cycles remain within valid range (0 to 1)
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

        // Move forward only if no obstacles or walls are detected
        if (!obstacle_detected && !wall_detected){
            motor_forward(duty_cycle_left, duty_cycle_right);
        }
        vTaskDelay(1000); // Delay for 1 second before the next iteration
    }
}


void IR_driver_task(__unused void *params)
{
    ir_sensor_init(); // Initialize IR sensors

    uint16_t leftResult, rightResult; // Variables to store sensor readings

    while (1) // Continuous operation
    {
        if (!obstacle_detected) // Check if no obstacle is detected
        {
            ir_sensor_read(&leftResult, &rightResult); // Read sensor values

            // Check conditions based on sensor readings
            if (leftResult > DETECTION_THRESHOLD && rightResult > DETECTION_THRESHOLD)
            {
                wall_detected = true; // Set flag for wall detection
                motor_stop(); // Stop the motor as both sensors detect a wall
            }
            else if (leftResult > DETECTION_THRESHOLD)
            {
                wall_detected = true; // Set flag for wall detection
                motor_rotate_right(duty_cycle_left, duty_cycle_right); // Rotate right as left sensor detects a wall
            }
            else if (rightResult > DETECTION_THRESHOLD)
            {
                wall_detected = true; // Set flag for wall detection
                motor_rotate_left(duty_cycle_left, duty_cycle_right); // Rotate left as right sensor detects a wall
            }
            else
            {
                wall_detected = false; // No obstacle detected, clear the wall detection flag
            }
        }
        else
        {
            wall_detected = false; // Clear the wall detection flag if an obstacle is detected elsewhere
        }
    }
}


void interruptHandler(uint gpio, uint32_t events)
{
    if (gpio == ULTRASONIC_ECHO_PIN){
        //ultrasonicHandler(events);
    }
    else if (gpio == LEFT_ENCODER_PIN){
        leftWheelEncoderHandler();
    }
    else if (gpio == RIGHT_ENCODER_PIN){
        rightWheelEncoderHandler();
    }
}

void setupInterrupts()
{
    gpio_set_irq_enabled_with_callback(ULTRASONIC_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &interruptHandler);
    gpio_set_irq_enabled(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true);

}

void vLaunch(void)
{
    // Declaring instances of our tasks
    // TaskHandle_t ledtask;
    TaskHandle_t motor_driverTask;
    TaskHandle_t ir_driverTask;

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