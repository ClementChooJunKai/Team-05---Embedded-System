include(FreeRTOS_Kernel_import.cmake)

include(web_interface/pico_sdk_import.cmake)

pico_sdk_init()

message("Running makefsdata python script")
execute_process(COMMAND
    py web_interface/makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
# Making executable file - assembled_car
add_executable(assembled_car assembled_car.c)

# Creating libraries based on headers+c files of the different drivers
add_library(car_magnetnometer car_magnetnometer/car_magnetnometer.c car_magnetnometer/car_magnetnometer.h)
add_library(car_ultrasonic car_ultrasonic/car_ultrasonic.c car_ultrasonic/car_ultrasonic.h)
add_library(car_wheel_driver car_wheel_driver/car_wheel_driver.c car_wheel_driver/car_wheel_driver.h)
add_library(car_irline car_irline/car_irline_sensor.c car_irline/car_irline_sensor.h)
add_library(car_wheel_encoder car_wheel_encoder/car_wheel_encoder.c car_wheel_encoder/car_wheel_encoder.h)



target_include_directories(assembled_car PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
)

# Link libraries for our individual drivers
target_link_libraries(car_magnetnometer pico_stdlib hardware_i2c)
target_link_libraries(car_ultrasonic pico_stdlib)
target_link_libraries(car_wheel_driver pico_stdlib hardware_pwm)
target_link_libraries(car_irline pico_stdlib hardware_adc)
target_link_libraries(car_wheel_encoder pico_stdlib)


target_compile_definitions(assembled_car PRIVATE
        # WIFI_SSID=\"${WIFI_SSID}\"
        # WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
        LWIP_SOCKET=1
        PICO_MAX_SHARED_IRQ_HANDLERS=5
        )

# pull in common dependencies for our assembled car.
target_link_libraries(  assembled_car 
                        pico_stdlib 
                        pico_lwip_iperf 
                        FreeRTOS-Kernel-Heap4 
                        pico_cyw43_arch_lwip_sys_freertos 
                        car_magnetnometer
                        car_ultrasonic
                        car_wheel_driver
                        car_wheel_encoder
                        pico_lwip_http
                        # hardware_adc
                        car_irline
                    )

# enable usb output onto serial monitor
pico_enable_stdio_usb(assembled_car 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(assembled_car)

# add url via pico_set_program_url
example_auto_set_url(assembled_car)
