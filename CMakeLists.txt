# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.20)

set(SUPPORTED_TARGETS esp32)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_climate_station)

idf_component_register(SRCS main.c bme_user.c bme280.c bme68x.c spi.c
                    INCLUDE_DIRS "." "../submodules/BME280_driver" "../submodules/BME68x-Sensor-API")

# Must define BME280/BME680
target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-DBME280")