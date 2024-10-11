#ifndef _ESP_MQTT_H_
#define _ESP_MQTT_H_

#include "mqtt_client.h"

// #define URI "mqtt://mqtt.flespi.io"
// #define ClientId "ESP32"
#define USERNAME "ah2cbuQ57EsGfyGVtW7939tlbBprFq17Gwdbie3jcRiG57SHrZduYBRGhotdrrcL"
#define PASSWORD ""
#define MAX_URI_LENGTH 256
#define MAX_CLIENT_ID_LENGTH 128
#define MAX_USERNAME_LENGTH 128
// #define MAX_PASSWORD_LENGTH 128
typedef enum mqtt_app_message
{
    MQTT_INIT = 0,
    MQTT_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
    MQTT_APP_MSG_CONNECTED,
    MQTT_APP_MSG_LOAD_SAVED_CREDENTIALS,
    MQTT_APP_MSG_USER_REQUESTED_DISCONNECT,
    MQTT_APP_MSG_DISCONNECTED,
} mqtt_app_message_e;
typedef struct mqtt_app_queue_message
{
    mqtt_app_message_e msgID;
} mqtt_app_queue_message_t;

void mqtt_start(void);
void mqtt_stop(void);
void Publisher_Task(char *topic, char *data);
esp_mqtt_client_config_t *mqtt_app_get_config(void);
BaseType_t mqtt_app_send_message(mqtt_app_message_e msgID);

#endif