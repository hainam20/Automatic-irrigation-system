#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "wifi.h"
#include "http_server_app.h"
#include "esp_mqtt.h"
#include "app_nvs.h"
#include "lora.h"
#include "cJSON.h"
static const char *TAG = "MQTT";

esp_mqtt_client_handle_t client = NULL;
esp_mqtt_client_config_t *mqtt_config = NULL;
static QueueHandle_t mqtt_app_queue_handle;

static EventGroupHandle_t mqtt_app_event_group;
const int MQTT_APP_CONNECTING_USING_SAVED_CREDS_BIT = BIT0;
const int MQTT_APP_CONNECTING_FROM_HTTP_SERVER_BIT = BIT1;
const int MQTT_APP_USER_REQUESTED_DISCONNECT_BIT = BIT2;
const int MQTT_APP_CONNECTED_BIT = BIT3;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{

    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRId32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        msg_id = esp_mqtt_client_subscribe(client, "/flespi/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/flespi/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        // mqtt_stop();
        mqtt_app_send_message(MQTT_APP_MSG_DISCONNECTED);
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // msg_id = esp_mqtt_client_publish(client, "/flespi/qos0", NULL, 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful");
        mqtt_app_send_message(MQTT_APP_MSG_CONNECTED);

        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (strcmp(event->topic, "/flespi/qos1") == 0)
        {
            uint8_t buf[256];
            int send_len = sprintf((char *)buf, event->data);
            // printf("%s \n", buf);
            send_lora_packet(buf, send_len);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
// static void mqtt_app_start(void)
// {
//     mqtt_config = (esp_mqtt_client_config_t *)malloc(sizeof(esp_mqtt_client_config_t));
//     memset(mqtt_config, 0x00, sizeof(esp_mqtt_client_config_t));

//     esp_mqtt_client_config_t mqtt_cfg = {
//         .broker.address.uri = URI,
//         .credentials.client_id = ClientId,
//         .credentials.username = Username,
//         .credentials.authentication.password = Password};

//     client = esp_mqtt_client_init(&mqtt_cfg);
//     /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
//     esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
//     esp_mqtt_client_start(client);
// }

esp_mqtt_client_config_t *mqtt_app_get_config(void)
{
    if (mqtt_config == NULL)
    {
        mqtt_config = (esp_mqtt_client_config_t *)malloc(sizeof(esp_mqtt_client_config_t));
    }
    return mqtt_config;
}
static void mqtt_app_connect(void)
{

    esp_mqtt_client_config_t *mqtt_cfg = mqtt_app_get_config();
    if (mqtt_cfg == NULL)
    {
        ESP_LOGE(TAG, "MQTT config is NULL");
        return;
    }
    printf("%s\n", mqtt_cfg->broker.address.uri);
    printf("%s\n", mqtt_cfg->credentials.username);
    printf("%s\n", mqtt_cfg->credentials.client_id);
    printf("%s\n", mqtt_cfg->credentials.authentication.password);
    client = esp_mqtt_client_init(mqtt_cfg);
    ESP_ERROR_CHECK(client == NULL ? ESP_FAIL : ESP_OK);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
    mqtt_app_send_message(MQTT_APP_MSG_CONNECTED);
}
static void mqtt_app_task(void *pvParameters)
{
    mqtt_app_queue_message_t msg;
    EventBits_t eventBits;
    mqtt_app_send_message(MQTT_APP_MSG_LOAD_SAVED_CREDENTIALS);
    for (;;)
    {
        if (xQueueReceive(mqtt_app_queue_handle, &msg, portMAX_DELAY))
        {
            switch (msg.msgID)
            {
            case MQTT_APP_MSG_LOAD_SAVED_CREDENTIALS:
                ESP_LOGI(TAG, "MQTT_APP_MSG_LOAD_SAVED_CREDENTIALS");

                if (app_nvs_load_mqtt_creds())
                {
                    ESP_LOGI(TAG, "Loaded MQTT configuration");
                    mqtt_app_connect();
                    xEventGroupSetBits(mqtt_app_event_group, MQTT_APP_CONNECTING_USING_SAVED_CREDS_BIT);
                }
                else
                {
                    ESP_LOGI(TAG, "Unable to load MQTT configuration");
                }
                break;
            case MQTT_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                ESP_LOGI(TAG, "MQTT_APP_MSG_CONNECTING_FROM_HTTP_SERVER");

                xEventGroupSetBits(mqtt_app_event_group, MQTT_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
                // Attempt a connection
                mqtt_app_connect();

                http_server_monitor_send_message(HTTP_MSG_MQTT_CONNECT_INIT);
                break;
            case MQTT_APP_MSG_CONNECTED:
                ESP_LOGI(TAG, "MQTT_APP_MSG_MQTT_CONNECTED");

                xEventGroupSetBits(mqtt_app_event_group, MQTT_APP_CONNECTED_BIT);

                http_server_monitor_send_message(HTTP_MSG_MQTT_CONNECT_SUCCESS);

                eventBits = xEventGroupGetBits(mqtt_app_event_group);
                if (eventBits & MQTT_APP_CONNECTING_USING_SAVED_CREDS_BIT) ///> Save STA creds only if connecting from the http server (not loaded from NVS)
                {
                    xEventGroupClearBits(mqtt_app_event_group, MQTT_APP_CONNECTING_USING_SAVED_CREDS_BIT); ///> Clear the bits, in case we want to disconnect and reconnect, then start again
                }
                else
                {
                    app_nvs_save_mqtt_creds();
                }
                if (eventBits & MQTT_APP_CONNECTING_FROM_HTTP_SERVER_BIT)
                {
                    xEventGroupClearBits(mqtt_app_event_group, MQTT_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
                }
                break;
            case MQTT_APP_MSG_USER_REQUESTED_DISCONNECT:
                ESP_LOGI(TAG, "MQTT_APP_MSG_USER_REQUESTED_DISCONNECT");

                eventBits = xEventGroupGetBits(mqtt_app_event_group);

                if (eventBits & MQTT_APP_CONNECTED_BIT)
                {
                    xEventGroupSetBits(mqtt_app_event_group, MQTT_APP_USER_REQUESTED_DISCONNECT_BIT);
                    ESP_ERROR_CHECK(esp_mqtt_client_stop(client));
                    app_nvs_clear_mqtt_creds();
                }
                break;
            case MQTT_APP_MSG_DISCONNECTED:
                ESP_LOGI(TAG, "MQTT_APP_MSG_DISCONNECTED");

                eventBits = xEventGroupGetBits(mqtt_app_event_group);
                if (eventBits & MQTT_APP_CONNECTING_USING_SAVED_CREDS_BIT)
                {
                    ESP_LOGI(TAG, "MQTT_APP_MSG_STA_DISCONNECTED: ATTEMPT USING SAVED CREDENTIALS");
                    xEventGroupClearBits(mqtt_app_event_group, MQTT_APP_CONNECTING_USING_SAVED_CREDS_BIT);
                    app_nvs_clear_mqtt_creds();
                }
                else if (eventBits & MQTT_APP_CONNECTING_FROM_HTTP_SERVER_BIT)
                {
                    ESP_LOGI(TAG, "MQTT_APP_MSG_STA_DISCONNECTED: ATTEMPT FROM THE HTTP SERVER");
                    xEventGroupClearBits(mqtt_app_event_group, MQTT_APP_CONNECTING_FROM_HTTP_SERVER_BIT);
                    http_server_monitor_send_message(HTTP_MSG_MQTT_CONNECT_FAIL);
                }
                else if (eventBits & MQTT_APP_USER_REQUESTED_DISCONNECT_BIT)
                {
                    ESP_LOGI(TAG, "MQTT_APP_MSG_DISCONNECTED: USER REQUESTED DISCONNECTION");
                    xEventGroupClearBits(mqtt_app_event_group, MQTT_APP_USER_REQUESTED_DISCONNECT_BIT);
                    http_server_monitor_send_message(HTTP_MSG_MQTT_DISCONNECT);
                }
                else
                {
                    ESP_LOGI(TAG, "MQTT_APP_MSG_DISCONNECTED: ATTEMPT FAILED, CHECK WIFI ACCESS POINT AVAILABILITY");
                    // Adjust this case to your needs - maybe you want to keep trying to connect...
                }
                if (eventBits & MQTT_APP_CONNECTED_BIT)
                {
                    xEventGroupClearBits(mqtt_app_event_group, MQTT_APP_CONNECTED_BIT);
                }
                break;

            default:
                break;
            }
        }
    }
}
BaseType_t mqtt_app_send_message(mqtt_app_message_e msgID)
{
    mqtt_app_queue_message_t msg;
    msg.msgID = msgID;
    return xQueueSend(mqtt_app_queue_handle, &msg, portMAX_DELAY);
}
void mqtt_start(void)
{
    ESP_LOGI(TAG, "STARTING MQTT APPLICATION");

    // Disable default WiFi logging messages
    // esp_log_level_set("MQTT", ESP_LOG_NONE);

    // Allocate memory for the MQTT configuration
    mqtt_config = (esp_mqtt_client_config_t *)malloc(sizeof(esp_mqtt_client_config_t));
    memset(mqtt_config, 0x00, sizeof(esp_mqtt_client_config_t));

    // Create message queue
    mqtt_app_queue_handle = xQueueCreate(3, sizeof(mqtt_app_queue_message_t));

    // Create MQTT application event group
    mqtt_app_event_group = xEventGroupCreate();

    // Start the MQTT application task
    xTaskCreatePinnedToCore(&mqtt_app_task, "mqtt_app_task", 4096, NULL, 6, NULL, 0);
}

void mqtt_stop(void)
{
    esp_err_t err = esp_mqtt_client_disconnect(client);
    if (err == ESP_OK)
    {
        printf("Disconnected mqtt\n");
    }
    else
    {
        printf("Failed to disconnect: %s\n", esp_err_to_name(err));
    }
    esp_mqtt_client_stop(client);
}
void Publisher_Task(char *topic, char *data)
{
    if (client != NULL)
    {
        esp_mqtt_client_publish(client, topic, data, 0, 0, 0);
    }
}
