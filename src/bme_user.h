#pragma once

#include "bme280.h"
#include "bme680.h"
#include <stdint.h>

void   user_delay_ms(uint32_t ms);
int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t* reg_data, uint16_t len);
int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t* reg_data, uint16_t len);

int8_t init_bme280(struct bme280_dev* dev);
int8_t init_bme680(struct bme680_dev* dev);
