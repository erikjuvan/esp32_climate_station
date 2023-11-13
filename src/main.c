#include "main.h"
#include "bme_user.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "spi.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(BME280)
typedef struct bme280_data bme_data_t;
typedef struct bme280_dev  bme_dev_t;
#elif defined(BME680)
typedef struct bme680_field_data bme_data_t;
typedef struct bme680_dev        bme_dev_t;
#endif

static const int sizeof_bme = sizeof(bme_data_t);
static bme_dev_t bme_dev;

static const char* TAG = "esp32_climate_station";

static bme_data_t get_bme_data()
{
    bme_data_t data;
#if defined(BME280)
    bme280_get_sensor_data(BME280_ALL, &data, &bme_dev);
#elif defined(BME680)
    bme680_get_sensor_data(&data, &bme_dev);
#endif
    return data;
}

/* WiFi should start before using ESPNOW */
static void example_wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());

#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
#endif
}

void app_main(void)
{
    init_spi();
#if defined(BME280)
    init_bme280(&bme_dev);
#elif defined(BME680)
    init_bme680(&bme_dev);
#endif

    //example_wifi_init();

	while (1)
	{
        bme_data_t bme_data = get_bme_data();

#if defined(BME280)
        printf("%.2f *C, %.1f %%rH, %d mBar\n", bme_data.temperature / 100.0, bme_data.humidity / 1024.f, bme_data.pressure / 10000);
#elif defined(BME680)
        printf("%.2f *C, %.1f %%rH, %d mBar, %d Ohm\n", bme_data.temperature / 100.0, bme_data.humidity / 1000.f, bme_data.pressure / 100, bme_data.gas_resistance);
#endif

        vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
