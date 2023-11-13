#pragma once

#include <stdint.h>

#if defined(BME280)
#include "bme280.h"
#elif defined(BME68x)
#include "bme68x.h"
#endif


void   user_delay_ms(uint32_t ms);
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t* reg_data, uint16_t len);
int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t* reg_data, uint16_t len);

#if defined(BME280)
int8_t init_bme280(struct bme280_dev* dev);
#elif defined(BME68x)
int8_t init_bme68x(struct bme68x_dev* dev);
#endif
