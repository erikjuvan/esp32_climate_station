#include <string.h>

#include "bme_user.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi.h"

#if defined(BME280)

#include "bme280.h"
static struct bme280_dev dev;

#elif defined(BME680)

#include "bme680.h"
static struct bme680_dev dev;

#endif

void user_delay_ms(uint32_t ms)
{
    // Return control or wait, for a period amount of milliseconds
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t* reg_data, uint16_t len)
{
    /*
     * The parameter dev_id can be used as a variable to select which Chip Select pin has
     * to be set low to activate the relevant device on the SPI bus
     */

    /*
     * Data on the bus should be like
     * |----------------+---------------------+-------------|
     * | MOSI           | MISO                | Chip Select |
     * |----------------+---------------------|-------------|
     * | (don't care)   | (don't care)        | HIGH        |
     * | (reg_addr)     | (don't care)        | LOW         |
     * | (don't care)   | (reg_data[0])       | LOW         |
     * | (....)         | (....)              | LOW         |
     * | (don't care)   | (reg_data[len - 1]) | LOW         |
     * | (don't care)   | (don't care)        | HIGH        |
     * |----------------+---------------------|-------------|
     */

    esp_err_t         ret; /* Return 0 for Success, non-zero for failure */
    spi_transaction_t t;

    memset(&t, 0, sizeof(t)); //Zero out the transaction

    t.length    = len * 8; // 8 bits = 1 byte (reg_addr).
    t.addr      = reg_addr;
    t.rx_buffer = reg_data; // Data
    t.user      = NULL;     // D/C needs to be set to 1

    ret = spi_device_polling_transmit(spi, &t); //Transmit!

    assert(ret == ESP_OK); //Should have had no issues.

    return ret;
}

int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t* reg_data, uint16_t len)
{
    /*
     * The parameter dev_id can be used as a variable to select which Chip Select pin has
     * to be set low to activate the relevant device on the SPI bus
     */

    /*
     * Data on the bus should be like
     * |---------------------+--------------+-------------|
     * | MOSI                | MISO         | Chip Select |
     * |---------------------+--------------|-------------|
     * | (don't care)        | (don't care) | HIGH        |
     * | (reg_addr)          | (don't care) | LOW         |
     * | (reg_data[0])       | (don't care) | LOW         |
     * | (....)              | (....)       | LOW         |
     * | (reg_data[len - 1]) | (don't care) | LOW         |
     * | (don't care)        | (don't care) | HIGH        |
     * |---------------------+--------------|-------------|
     */

    esp_err_t         ret; /* Return 0 for Success, non-zero for failure */
    spi_transaction_t t;

    if (len == 0)
        return 0; //no need to send anything

    memset(&t, 0, sizeof(t)); //Zero out the transaction

    t.length    = len * 8; // Len is in bytes, transaction length is in bits.
    t.addr      = reg_addr;
    t.tx_buffer = reg_data; // Data
    t.user      = (void*)1; // D/C needs to be set to 1

    ret = spi_device_polling_transmit(spi, &t); //Transmit!

    assert(ret == ESP_OK); //Should have had no issues.

    return ret;
}

int8_t init_bme()
{
    int8_t rslt = 0;

    /* Sensor_0 interface over SPI with native chip select line */
    dev.dev_id   = 0;
    dev.intf     = 0; // BME2(6)80_SPI_INTF;
    dev.read     = user_spi_read;
    dev.write    = user_spi_write;
    dev.delay_ms = user_delay_ms;

#if defined(BME280)
    rslt = bme280_init(&dev);
#elif defined(BME680)
    dev.amb_temp = 25;
    rslt         = bme680_init(&dev);
#endif

    return rslt;
}