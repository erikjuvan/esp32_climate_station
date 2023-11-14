#include <math.h>
#include <stdint.h>
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <esp_sleep.h>

#define DEVICE_ID 1

#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme(BME_CS); // hardware SPI

const char* ssid = "TP-Link_5C3F";
const char* password = "nina6988jeslapoelo2104";
const char* serverIP = "192.168.0.106";
const int serverPort = 8888;

WiFiClient client;

uint32_t time_get()
{
    return millis();
}

uint32_t time_get_elapsed(uint32_t start)
{
    return time_get() - start;
}

bool time_is_elapsed(uint32_t start, uint32_t elapsed)
{
    return time_get_elapsed(start) >= elapsed;
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    // Serial.println("Connected to WiFi");
    // Serial.println("IP: " + WiFi.localIP());
    

    // BME
    unsigned status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status)
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        
        while (1)
        {
            delay(10);
        }
    }
}

bool connect_to_server()
{
    if (!client.connected())
    {
        Serial.println("Connecting to server");
        if (client.connect(serverIP, serverPort))
        {
            Serial.println("Connected");
        }
        else
        {
            Serial.println("Connection failed");
            return false;
        }
    }

    return true;
}

void send_data_to_server()
{
    static uint32_t cnt = 0;
    String data = "ID" + String(DEVICE_ID) + "," + String(cnt++) + "," + collect_bme280_data();
    Serial.println(data);
    client.println(data);
    client.flush();
}

void enter_deep_sleep()
{
    // Disable WiFi
    // esp_wifi_stop();

    // Enter deep sleep
    const int SLEEP_DURATION_MIN = 1;
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_MIN * 60 * 1000000); // microseconds
    esp_deep_sleep_start();
    
    // After waking up, re-enable WiFi and perform necessary initialization
    // esp_wifi_start();
}

void loop()
{
    if (connect_to_server() == false)
    {
        delay(5000);
        return;
    }

    send_data_to_server();
    
    // enter_deep_sleep();
    delay(2000);

    // setup();
}

String collect_bme280_data()
{
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;
    float absoluteHumidity = calculate_absolute_humidity(temperature, humidity);
    float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

    String data = String(temperature) + "," + String(humidity) + "," + String(pressure) + "," + String(absoluteHumidity) + "," + String(altitude);

    return data;
}

float calculate_absolute_humidity(float temperature, float relativeHumidity)
{
    const float T = temperature;
    const float RH = relativeHumidity;
    const float C = 2.16679f;
    const float Tk = T + 273.15f;
    const float A = 6.116441f;
    const float m = 7.591386f;
    const float Tn = 240.7263f;
    const float Pws_hPa = A * powf(10, m * T /(T + Tn));
    const float Pw_hPa = (RH / 100.f) * Pws_hPa;
    const float Pw_Pa = Pw_hPa * 100;

    const float AH = C * Pw_Pa / Tk;
  
    return AH;
}
