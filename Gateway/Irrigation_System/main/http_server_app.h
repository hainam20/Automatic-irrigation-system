#ifndef _HTTP_SERVER_APP_H
#define _HTTP_SERVER_APP_H

#include "freertos/FreeRTOS.h"
typedef void (*http_get_callback_t)(void);

typedef enum http_server_wifi_connect_status
{
    NONE = 0,
    HTTP_WIFI_STATUS_CONNECTING,
    HTTP_WIFI_STATUS_CONNECT_FAILED,
    HTTP_WIFI_STATUS_CONNECT_SUCCESS,
    HTTP_WIFI_STATUS_DISCONNECTED,
    HTTP_MQTT_STATUS_CONNECTING,
    HTTP_MQTT_STATUS_CONNECT_FAILED,
    HTTP_MQTT_STATUS_CONNECT_SUCCESS,
    HTTP_MQTT_STATUS_DISCONNECTED,
} http_server_wifi_connect_status_e;

/**
 * Messages for the HTTP monitor
 */
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAIL,
    HTTP_MSG_WIFI_USER_DISCONNECT,
    HTTP_MSG_MQTT_CONNECT_INIT,
    HTTP_MSG_MQTT_CONNECT_SUCCESS,
    HTTP_MSG_MQTT_CONNECT_FAIL,
    HTTP_MSG_MQTT_DISCONNECT,

} http_server_message_e;

/**
 * Structure for the message queue
 */
typedef struct http_server_queue_message
{
    http_server_message_e msgID;
} http_server_queue_message_t;

BaseType_t http_server_monitor_send_message(http_server_message_e msgID);
void start_webserver(void);
void stop_webserver(void);
void http_set_json_callback(void *cb);
// void json_response(char *data, int len);
#endif