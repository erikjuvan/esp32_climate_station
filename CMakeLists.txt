# Minimum required version of CMake
cmake_minimum_required(VERSION 3.20)

# Project name
project(esp32_climate_station)

# Include the ESP-IDF CMake build system
set(SUPPORTED_TARGETS esp32)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Register the components of your project
idf_component_register(
    SRCS 
    main.c
    bme_user.c
    bme280.c
    bme68x.c
    spi.c

    INCLUDE_DIRS
    "."
    "../submodules/BME280_driver"
    "../submodules/BME68x-Sensor-API"
)

# Define compilation flags or options if needed
target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-DBME280")
