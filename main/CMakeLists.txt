idf_component_register(SRCS esp32_climate_station.c bme_user.c bme280.c bme680.c spi.c uart.c
                    INCLUDE_DIRS ".")

# Must define BME280/BME680 and SERVER/CLIENT
target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-DBME280" "-DCLIENT")