# Team-05---Embedded-System

This GitHub repository contains the source code and documentation for a project aimed at creating an intelligent autonomous 2-wheel robot car. The project utilizes the Raspberry Pi Pico microcontroller, various sensors, and a PID (Proportional-Integral-Derivative) controller to achieve efficient navigation, obstacle avoidance, and barcode recognition.

Project Description
The primary goal of this project is to develop a robot car capable of navigating a predefined track, efficiently avoiding obstacles, and recognizing barcodes using infrared sensors. The integration of a PID controller enhances the car's control system, ensuring smooth and accurate motion during navigation and barcode scanning.

| Feature              | Description                                                                                                     |
|----------------------|-----------------------------------------------------------------------------------------------------------------|
| Motor Sensor         | Initiates simple movements in various directions: left, right, up, and down using the motor.                    |
| PID Controller       | Employs a PID controller and an encoder to regulate and control speed.                                           |
| Mapping and Navigation| Converts mazes into printable maps and finds the shortest path based on the generated map.                       |
| Ultrasonic           | Determines minimum and maximum distances to obstacles using an ultrasonic sensor.                                 |
| IR Sensors           | Utilizes infrared sensors for line tracking and barcode detection.                                                 |
| Magnetometer         | Utilizes a magnetometer to obtain compass bearing values.                                                           |
| Filter Data          | Applies filtering techniques to improve accuracy and precision of sensor data.                                    |


## GPIO Pins
| `Pin Number`| `Usage`       | `Sensor`      |
| ------------|:-------------:|:-------------:|
| GPIO 19      | ENB           | L298N Module |
| GPIO 12      | ENA           | L298N Module |
| GPIO 4      | SDA           | Magnetometer |
| GPIO 5      | SCL           | Magnetometer |
| GPIO 21     | IN4           | L298N Module |
| GPIO 20     | IN3           | L298N Module |
| GPIO 11     | IN2           | L298N Module |
| GPIO 10     | IN1           | L298N Module |
| GPIO 16     | TRIG          | Ultrasonic |
| GPIO 17     | ECHO          | Ultrasonic |
| GPIO 2     | OUT           | Left Encoder |
| GPIO 3     | OUT           | Right Encoder |
| GPIO 27     | AO            | Right IR |
| GPIO 26     | AO            | Left IR |
| GPIO 28     | AO            |  Barcode IR |

For diagrams, please refer to the 'diagrams' folder.
