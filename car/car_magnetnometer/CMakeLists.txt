# add_executable(car_magnetnometer
# car_magnetnometer.c
#         )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(car_magnetnometer pico_stdlib hardware_i2c)

pico_enable_stdio_usb(car_magnetnometer 1)
pico_enable_stdio_uart(car_magnetnometer 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(car_magnetnometer)

# add url via pico_set_program_url
example_auto_set_url(car_magnetnometer)
