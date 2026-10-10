#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define NTC_ADC_CHANNEL ADC_CHANNEL_0  // GPIO0 on ESP32-C3

#define R_FIXED  10000.0f
#define R0       10000.0f
#define T0_K     298.15f
#define BETA     3950.0f

#define WARN_TEMP_C  50.0f

#define LED_GPIO GPIO_NUM_4

static const char *TAG_SENSOR = "SENSOR";
static const char *TAG_SAFETY = "SAFETY";
static const char *TAG_MAIN   = "MAIN";

typedef struct {
    int adc_raw;
    float temperature_c;
} SensorData;

static QueueHandle_t temperature_queue;

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t cali_handle;
static bool calibration_ok = false;

static void sensor_task(void *arg)
{
    while (1) {
        int raw = 0;
        int mv = 0;

        if (adc_oneshot_read(adc_handle, NTC_ADC_CHANNEL, &raw) == ESP_OK) {
            float voltage;
            float resistance;
            float temp_k;
            float temp_c;

            if (calibration_ok &&
                adc_cali_raw_to_voltage(cali_handle, raw, &mv) == ESP_OK) {
                voltage = mv / 1000.0f;

                // Circuit: 3V3 -> NTC -> GPIO0 -> fixed 10K -> GND
                if (voltage > 0.01f && voltage < 3.29f) {
                    resistance = R_FIXED * voltage / (3.3f - voltage);

                    temp_k = 1.0f /
                        ((1.0f / T0_K) +
                         (logf(resistance / R0) / BETA));

                    temp_c = temp_k - 273.15f;

                    SensorData data = {
                        .adc_raw = raw,
                        .temperature_c = temp_c
                    };

                    // Copy this reading into the queue.
                    xQueueSend(temperature_queue, &data, portMAX_DELAY);
                } else {
                    ESP_LOGW(TAG_SENSOR,
                             "Voltage out of range: %.2f V", voltage);
                }
            } else {
                ESP_LOGW(TAG_SENSOR,
                         "ADC calibration unavailable; raw ADC = %d", raw);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void safety_task(void *arg)
{
    SensorData data;

    while (1) {
        // Wait until the sensor task sends a reading.
        if (xQueueReceive(temperature_queue, &data, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG_SAFETY,
                     "Queue received: ADC=%d, Temperature=%.1f C",
                     data.adc_raw, data.temperature_c);

            if (data.temperature_c >= WARN_TEMP_C) {
                gpio_set_level(LED_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(500));

                gpio_set_level(LED_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(500));

                ESP_LOGW(TAG_SAFETY, "WARNING: HIGH TEMPERATURE!");
            } else {
                gpio_set_level(LED_GPIO, 0);
                ESP_LOGI(TAG_SAFETY, "Temperature normal");
            }
        }
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(gpio_reset_pin(LED_GPIO));

    ESP_ERROR_CHECK(
        gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT)
    );

    ESP_ERROR_CHECK(
        gpio_set_level(LED_GPIO, 0)
    );

    temperature_queue = xQueueCreate(5, sizeof(SensorData));

    if (temperature_queue == NULL) {
        ESP_LOGE(TAG_MAIN, "Queue creation failed!");
        return;
    }

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(
        adc_handle, NTC_ADC_CHANNEL, &chan_cfg));

    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    if (adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali_handle) == ESP_OK) {
        calibration_ok = true;
    }

    xTaskCreate(sensor_task, "Sensor", 3072, NULL, 2, NULL);
    xTaskCreate(safety_task, "Safety", 3072, NULL, 2, NULL);

    ESP_LOGI(TAG_MAIN, "Temperature monitor started");
}
