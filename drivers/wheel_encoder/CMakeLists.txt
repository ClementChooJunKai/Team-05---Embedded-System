add_executable(wheel_encoder wheel_encoder.c)

# pull in common dependencies
target_link_libraries(wheel_encoder pico_stdlib)

# enable usb output, enable uart output
pico_enable_stdio_usb(wheel_encoder 1)
pico_enable_stdio_uart(wheel_encoder 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(wheel_encoder)

# add url via pico_set_program_url
example_auto_set_url(wheel_encoder)