#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define DIP_1 GPIO_NUM_0
#define DIP_2 GPIO_NUM_1
#define DIP_3 GPIO_NUM_3
#define DIP_4 GPIO_NUM_4

#define LED_GPIO GPIO_NUM_5

static const char *TAG = "DIP_SEMAPHORE";

static SemaphoreHandle_t bell;

// TASK A: Read DIP switches and check the code
static void task_a(void *arg)
{
    bool code_was_correct = false;

    while (1)
    {
        // Read switches: ON = 1, OFF = 0
        int dip_1 = (gpio_get_level(DIP_1) == 0) ? 1 : 0;
        int dip_2 = (gpio_get_level(DIP_2) == 0) ? 1 : 0;
        int dip_3 = (gpio_get_level(DIP_3) == 0) ? 1 : 0;
        int dip_4 = (gpio_get_level(DIP_4) == 0) ? 1 : 0;
	
	ESP_LOGI(TAG, "DIP = %d%d%d%d", dip_1, dip_2, dip_3, dip_4);

        if (dip_1 == 0 && dip_2 == 0 &&
            dip_3 == 0 && dip_4 == 0)
        {
            if (!code_was_correct)
            {
                ESP_LOGI(TAG, "Correct code! Giving semaphore");

                xSemaphoreGive(bell);
                code_was_correct = true;
            }
        }
        else
        {
            // Allow another attempt after code changes
            code_was_correct = false;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// TASK B: Wait for semaphore and turn LED ON
static void task_b(void *arg)
{
    while (1)
    {
        if (xSemaphoreTake(bell, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "Semaphore received! Blinking LED");

            for (int i = 0; i <5; i++)
            {
                gpio_set_level(LED_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(500));

                gpio_set_level(LED_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(500));
            }

            ESP_LOGI(TAG, "Blinking complete. Waiting for next trigger.");
        }
    }
}

void app_main(void)
{
    // Configure LED
    ESP_ERROR_CHECK(gpio_reset_pin(LED_GPIO));
    ESP_ERROR_CHECK(gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, 0));

    // Configure DIP switches
    ESP_ERROR_CHECK(gpio_reset_pin(DIP_1));
    ESP_ERROR_CHECK(gpio_set_direction(DIP_1, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_pull_mode(DIP_1, GPIO_PULLUP_ONLY));

    ESP_ERROR_CHECK(gpio_reset_pin(DIP_2));
    ESP_ERROR_CHECK(gpio_set_direction(DIP_2, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_pull_mode(DIP_2, GPIO_PULLUP_ONLY));

    ESP_ERROR_CHECK(gpio_reset_pin(DIP_3));
    ESP_ERROR_CHECK(gpio_set_direction(DIP_3, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_pull_mode(DIP_3, GPIO_PULLUP_ONLY));

    ESP_ERROR_CHECK(gpio_reset_pin(DIP_4));
    ESP_ERROR_CHECK(gpio_set_direction(DIP_4, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_pull_mode(DIP_4, GPIO_PULLUP_ONLY));

    // Create empty binary semaphore
    bell = xSemaphoreCreateBinary();

    if (bell == NULL)
    {
        ESP_LOGE(TAG, "Semaphore creation failed");
        return;
    }

    // Create tasks
    xTaskCreate(task_a, "TaskA", 3072, NULL, 2, NULL);
    xTaskCreate(task_b, "TaskB", 3072, NULL, 2, NULL);

    ESP_LOGI(TAG, "Enter the right DIP code");
}
