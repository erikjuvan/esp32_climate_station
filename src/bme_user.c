#include "bme_user.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi.h"
#include <string.h>

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

    spi_device_handle_t spi = *get_spi_handle();

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

    spi_device_handle_t spi = *get_spi_handle();

    ret = spi_device_polling_transmit(spi, &t); //Transmit!

    assert(ret == ESP_OK); //Should have had no issues.

    return ret;
}

int8_t init_bme280(struct bme280_dev* dev)
{
    int8_t  rslt = 0;
    uint8_t settings_sel;

    /* Sensor_0 interface over SPI with native chip select line */
    dev->dev_id   = 0;
    dev->intf     = BME280_SPI_INTF;
    dev->read     = user_spi_read;
    dev->write    = user_spi_write;
    dev->delay_ms = user_delay_ms;

    rslt = bme280_init(dev);

    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h        = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p        = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t        = BME280_OVERSAMPLING_2X;
    dev->settings.filter       = BME280_FILTER_COEFF_2;
    dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    settings_sel = BME280_OSR_PRESS_SEL;
    settings_sel |= BME280_OSR_TEMP_SEL;
    settings_sel |= BME280_OSR_HUM_SEL;
    settings_sel |= BME280_STANDBY_SEL;
    settings_sel |= BME280_FILTER_SEL;
    rslt = bme280_set_sensor_settings(settings_sel, dev);
    rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

    return rslt;
}

int8_t init_bme68x(struct bme68x_dev* dev)
{
    int8_t  rslt = 0;
    uint8_t set_required_settings;

    // Sensor_0 interface over SPI with native chip select line */
    dev->dev_id = 0;
    dev->intf   = BME68x_SPI_INTF;
    ;
    dev->read     = user_spi_read;
    dev->write    = user_spi_write;
    dev->delay_ms = user_delay_ms;
    dev->amb_temp = 25;

    rslt = bme68x_init(dev);

    // Set the temperature, pressure and humidity settings
    dev->tph_sett.os_hum  = BME68x_OS_2X;
    dev->tph_sett.os_pres = BME68x_OS_4X;
    dev->tph_sett.os_temp = BME68x_OS_8X;
    dev->tph_sett.filter  = BME68x_FILTER_SIZE_3;

    // Set the remaining gas sensor settings and link the heating profile
    dev->gas_sett.run_gas = BME68x_ENABLE_GAS_MEAS;
    // Create a ramp heat waveform in 3 steps
    dev->gas_sett.heatr_temp = 320; // degree Celsius
    dev->gas_sett.heatr_dur  = 150; // milliseconds

    // Select the power mode
    // Must be set before writing the sensor configuration
    dev->power_mode = BME68x_FORCED_MODE;

    // Set the required sensor settings needed */
    set_required_settings = BME68x_OST_SEL | BME68x_OSP_SEL | BME68x_OSH_SEL | BME68x_FILTER_SEL | BME68x_GAS_SENSOR_SEL;

    // Set the desired sensor configuration */
    rslt = bme68x_set_sensor_settings(set_required_settings, dev);

    // Set the power mode */
    rslt = bme68x_set_sensor_mode(dev);

    return rslt;
}
