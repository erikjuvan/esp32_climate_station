idf_component_register(SRCS main.c bme_user.c bme280.c bme680.c spi.c
                    INCLUDE_DIRS ".")

# Must define BME280/BME680
target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-DBME280")