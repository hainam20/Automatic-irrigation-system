

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "string.h"
#include "cJson.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "lora.h"
#include "wifi.h"
#include "DHT22.h"
#include "esp_mqtt.h"
#include "app_nvs.h"

uint8_t buf[256];
extern bool check_send;
extern int quantity;
static int numberSend = 0;
static const char *TAG = "Timer";

void timer_callback(void *arg)
{
    int i = 0;
    while (i < quantity)
    {
        uint8_t get[256];
        int send_len = sprintf((char *)get, "GET_ID%d", i); //"GET_ID%d" "{\"ID\": %d, \"Relay\": 1}"
        lora_send_data(get, send_len, i);
        vTaskDelay(pdMS_TO_TICKS(5000));
        memset(get, 0, sizeof(get));
        i++;
    }
    numberSend++;
    // printf("Number of packets: %d\n", numberSend);
    // for (int i = 0; i < 3; i++)
    // {
    //     uint8_t get[256];
    //     int send_len = sprintf((char *)get, "GET_ID%d", i);
    //     send_lora_packet(get, send_len);
    //     memset(get, 0, sizeof(get));
    // }
    ESP_LOGI(TAG, "1 hour has passed!");
}
// void task_tx(void *pvParameters)
// {
//     ESP_LOGI(pcTaskGetName(NULL), "Start");
//     uint8_t buf[256];
//     int i = 0;
//     while (1)
//     {
//         if (i < 3)
//         {
//             int send_len = sprintf((char *)buf, "GET_ID%d", i);
//             lora_send_packet(buf, send_len);
//             memset(buf, 0, sizeof(buf));
//             vTaskDelay(pdMS_TO_TICKS(10000));
//             i++;
//         }
//         else
//         {
//             i = 0;
//         }

//     } // end while
// }

// void timer_callback(void *arg)
// {
// 	// Hàm callback sau khi timer kết thúc
// }
void task_rx(void *pvParameters)
{
    while (1)
    {
        receive_lora_packet();
        vTaskDelay(pdMS_TO_TICKS(1)); // Avoid WatchDog alerts and allow other tasks to run
    }
}

void app_main()
{

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    int cr = 5;
    int sf = 7;
    if (lora_init() == 0)
    {
        ESP_LOGE(pcTaskGetName(NULL), "Does not recognize the module");
        while (1)
        {
            vTaskDelay(1);
        }
    }
    lora_set_frequency(433e6);
    lora_enable_crc();
    lora_set_coding_rate(cr);
    lora_set_bandwidth(125E3); // 125E3;
    lora_set_spreading_factor(sf);
    // lora_set_sync_word(0xF3);
    //
    wifi_app_start();
    // mqtt_start();
    if (get_quantity_from_nvs(&quantity))
    {
        printf("Quantity: %d\n", quantity);
    }
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "hourly_timer"};
    esp_timer_handle_t timer_handle;

    esp_timer_create(&timer_args, &timer_handle);
    esp_timer_start_periodic(timer_handle, 30000000LL); // 3600000000LL = 1 ho ur
    xTaskCreatePinnedToCore(task_rx, "task_rx", 4096, NULL, 5, NULL, 1);
    // TimerHandle_t xTimer = xTimerCreate("timer", pdMS_TO_TICKS(3600000), pdTRUE, 0, timer_callback); // 1 hour timer
    // if (xTimer == NULL)
    // {
    //     ESP_LOGE("app_main", "Failed to create timer");
    // }
    // else
    // {
    //     xTimerStart(xTimer, 0);
    // }
    // xTaskCreate(&task_rx, "task_rx", 1024 * 3, NULL, 5, NULL);
    // xTaskCreatePinnedToCore(&task_rx, "task_rx", 1024 * 3, NULL, 5 , 1);
}
