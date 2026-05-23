cmake_minimum_required(VERSION 3.13)

# Resolve Pico SDK path from env or local copy
if(DEFINED ENV{PICO_SDK_PATH})
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
else()
    include(pico_sdk_import.cmake)
endif()

# Resolve Pimoroni path from env or cmake arg
if(NOT DEFINED PIMORONI_PICO_PATH)
    if(DEFINED ENV{PIMORONI_PICO_PATH})
        set(PIMORONI_PICO_PATH $ENV{PIMORONI_PICO_PATH})
    else()
        message(FATAL_ERROR "Set PIMORONI_PICO_PATH env var or pass -DPIMORONI_PICO_PATH=...")
    endif()
endif()
include(${PIMORONI_PICO_PATH}/pimoroni_pico_import.cmake)

project(hello_world C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include(${PIMORONI_PICO_PATH}/common/pimoroni_bus.cmake)
include(${PIMORONI_PICO_PATH}/libraries/pico_explorer/pico_explorer.cmake)

add_executable(hello_world
    hello_world.cpp
    display.cpp
    text_screen.cpp
)

target_link_libraries(hello_world
    pico_stdlib
    pico_explorer
)

# USB serial output; no UART
pico_enable_stdio_usb(hello_world 1)
pico_enable_stdio_uart(hello_world 0)

# Generate .uf2, .hex, .bin, .map
pico_add_extra_outputs(hello_world)
