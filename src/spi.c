#include "spi.h"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5

static spi_device_handle_t spi_handle;

esp_err_t init_spi()
{
    esp_err_t        ret;

    //Initialize the SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num     = PIN_NUM_MISO,
        .mosi_io_num     = PIN_NUM_MOSI,
        .sclk_io_num     = PIN_NUM_CLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = 100};
        
    ret = spi_bus_initialize(VSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);

    //Attach the sensor to the SPI bus
    spi_device_interface_config_t devcfg = {
        .address_bits   = 8,
        .clock_speed_hz = 1 * 1000 * 1000, // Clock out at 1 MHz
        .mode           = 0,               // SPI mode 0
        .spics_io_num   = PIN_NUM_CS,      // CS pin
        .queue_size     = 1,               // We want to be able to queue 1 transactions at a time
                                           //.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA
    };

    ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
    ESP_ERROR_CHECK(ret);

    return ret;
}

const spi_device_handle_t* get_spi_handle(void)
{
    return &spi_handle;
}