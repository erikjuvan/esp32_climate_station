#pragma once

#include "driver/spi_master.h"

esp_err_t init_spi(void);
const spi_device_handle_t* get_spi_handle(void);
