#include "http_server_app.h"

#include <stdint.h>
#include "esp_mac.h"
#include <esp_log.h>
#include <esp_system.h>

#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "wifi.h"
#include "esp_mqtt.h"
#include <cJSON.h>
#include "app_nvs.h"
static const char *TAG = "HTTP_SERVER";

#define MAX_RECV_BUF 256
static httpd_handle_t http_server_handle = NULL;

// static httpd_req_t *REG;

static int g_wifi_connect_status = NONE;
static int g_mqtt_connect_status = NONE;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_queue_handle;

// HTTP server monitor task handle
static TaskHandle_t task_http_server_monitor = NULL;

extern const uint8_t jquery_3_3_1_min_js_start[] asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[] asm("_binary_jquery_3_3_1_min_js_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t app_css_start[] asm("_binary_app_css_start");
extern const uint8_t app_css_end[] asm("_binary_app_css_end");
extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[] asm("_binary_app_js_end");
extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");
/**
 * Set JSON callback
 */
static http_get_callback_t http_get_json_callback = NULL;
int quantity = 0;
/**
 * HTTP server monitor task used to track events of the HTTP server
 * @param pvParameters parameter which can be passed to the task.
 */
static void http_server_monitor(void *parameter)
{
    http_server_queue_message_t msg;

    for (;;)
    {
        if (xQueueReceive(http_server_monitor_queue_handle, &msg, portMAX_DELAY))
        {
            switch (msg.msgID)
            {
            case HTTP_MSG_WIFI_CONNECT_INIT:
                ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_INIT");

                g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECTING;
                break;

            case HTTP_MSG_WIFI_CONNECT_SUCCESS:
                ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");

                g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECT_SUCCESS;

                break;

            case HTTP_MSG_WIFI_CONNECT_FAIL:
                ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");

                g_wifi_connect_status = HTTP_WIFI_STATUS_CONNECT_FAILED;

                break;

            case HTTP_MSG_WIFI_USER_DISCONNECT:
                ESP_LOGI(TAG, "HTTP_MSG_WIFI_USER_DISCONNECT");

                g_wifi_connect_status = HTTP_WIFI_STATUS_DISCONNECTED;

                break;
            case HTTP_MSG_MQTT_CONNECT_INIT:
                ESP_LOGI(TAG, "HTTP_MSG_MQTT_CONNECT_INIT");

                g_mqtt_connect_status = HTTP_MQTT_STATUS_CONNECTING;
                break;

            case HTTP_MSG_MQTT_CONNECT_SUCCESS:
                ESP_LOGI(TAG, "HTTP_MSG_MQTT_CONNECT_SUCCESS");

                g_mqtt_connect_status = HTTP_MQTT_STATUS_CONNECT_SUCCESS;

                break;

            case HTTP_MSG_MQTT_CONNECT_FAIL:
                ESP_LOGI(TAG, "HTTP_MSG_MQTT_CONNECT_FAIL");

                g_mqtt_connect_status = HTTP_MQTT_STATUS_CONNECT_FAILED;

                break;

            case HTTP_MSG_MQTT_DISCONNECT:
                ESP_LOGI(TAG, "HTTP_MSG_MQTT_USER_DISCONNECT");

                g_mqtt_connect_status = HTTP_MQTT_STATUS_DISCONNECTED;

                break;

            default:
                break;
            }
        }
    }
}

/**
 * Jquery get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */

static esp_err_t http_server_jquery_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Jquery requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)jquery_3_3_1_min_js_start, jquery_3_3_1_min_js_end - jquery_3_3_1_min_js_start);

    return ESP_OK;
}
/**
 * Sends the index.html page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "index.html requested");

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);

    return ESP_OK;
}
/**
 * app.css get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "app.css requested");

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);

    return ESP_OK;
}

/**
 * wifiConnect.json handler is invoked after the connect button is pressed
 * and handles receiving the SSID and password entered by the user
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_wifi_connect_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/wifiConnect.json requested");

    char recv_buf[MAX_RECV_BUF] = {0};
    int ret = httpd_req_recv(req, recv_buf, MAX_RECV_BUF - 1);
    if (ret <= 0)
    {
        // If no data is received or an error occurred
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    // Parse the JSON data
    cJSON *json = cJSON_Parse(recv_buf);

    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }
    // Get SSID header
    cJSON *ssid_json = cJSON_GetObjectItemCaseSensitive(json, "ssid");
    if (!cJSON_IsString(ssid_json) || (ssid_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "SSID not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SSID not found or not a string");
        return ESP_FAIL;
    }
    // Get Password header
    cJSON *password_json = cJSON_GetObjectItemCaseSensitive(json, "password");
    if (!cJSON_IsString(password_json) || (password_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "Password not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Password not found or not a string");
        return ESP_FAIL;
    }

    char *ssid_str = strdup(ssid_json->valuestring);
    char *pass_str = strdup(password_json->valuestring);
    size_t len_ssid = strlen(ssid_str), len_pass = strlen(pass_str);
    // Update the Wifi networks configuration and let the wifi application

    wifi_config_t *wifi_config = wifi_app_get_wifi_config();
    memset(wifi_config, 0x00, sizeof(wifi_config_t));
    memcpy(wifi_config->sta.ssid, ssid_str, len_ssid);
    memcpy(wifi_config->sta.password, pass_str, len_pass);
    wifi_app_send_message(WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER);

    free(ssid_str);
    free(pass_str);

    return ESP_OK;
}
/**
 * app.js get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "app.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);

    return ESP_OK;
}

/**
 * wifiConnectStatus handler updates the connection status for the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_wifi_connect_status_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/wifiConnectStatus requested");

    char statusJSON[100];

    sprintf(statusJSON, "{\"wifi_connect_status\":%d}", g_wifi_connect_status);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, statusJSON, strlen(statusJSON));

    return ESP_OK;
}

/**
 * wifiDisconnect.json handler responds by sending a message to the Wifi application to disconnect.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_wifi_disconnect_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "wifiDisconect.json requested");

    wifi_app_send_message(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);

    return ESP_OK;
}

static esp_err_t http_server_mqtt_connect_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "mqttConnect.json requested");

    char recv_buf[MAX_RECV_BUF] = {0};
    int ret = httpd_req_recv(req, recv_buf, MAX_RECV_BUF - 1);
    if (ret <= 0)
    {
        // If no data is received or an error occurred
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    // Parse the JSON data
    cJSON *json = cJSON_Parse(recv_buf);

    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }
    // Get SSID header
    cJSON *uri_json = cJSON_GetObjectItemCaseSensitive(json, "URI");
    if (!cJSON_IsString(uri_json) || (uri_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "URI not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "URI not found or not a string");
        return ESP_FAIL;
    }
    // Get ClientId
    cJSON *clientid_json = cJSON_GetObjectItemCaseSensitive(json, "ClientId");
    if (!cJSON_IsString(clientid_json) || (clientid_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "ClientId not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SSID not found or not a string");
        return ESP_FAIL;
    }
    // Get Username
    cJSON *username_json = cJSON_GetObjectItemCaseSensitive(json, "Username");
    if (!cJSON_IsString(username_json) || (username_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "Username not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "SSID not found or not a string");
        return ESP_FAIL;
    }
    // Get Password header
    cJSON *password_json = cJSON_GetObjectItemCaseSensitive(json, "Password");
    if (!cJSON_IsString(password_json) || (password_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "Password not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Password not found or not a string");
        return ESP_FAIL;
    }

    char *URI = strdup(uri_json->valuestring);
    char *ClientId = strdup(clientid_json->valuestring);
    char *Username = strdup(username_json->valuestring);
    char *Password = strdup(password_json->valuestring);
    // size_t len_uri = strlen(URI), len_clientid = strlen(ClientId);
    // size_t len_username = strlen(Username), len_pass = strlen(Password);

    // Update the Wifi networks configuration and let the wifi application

    esp_mqtt_client_config_t *mqtt_cfg = mqtt_app_get_config();
    memset(mqtt_cfg, 0x00, sizeof(esp_mqtt_client_config_t));
    mqtt_cfg->broker.address.uri = strdup(URI);
    mqtt_cfg->credentials.client_id = strdup(ClientId);
    mqtt_cfg->credentials.username = strdup(Username);
    mqtt_cfg->credentials.authentication.password = strdup(Password);

    mqtt_app_send_message(MQTT_APP_MSG_CONNECTING_FROM_HTTP_SERVER);

    free(URI);
    free(ClientId);
    free(Username);
    free(Password);

    return ESP_OK;
}

static esp_err_t http_server_mqtt_connect_status_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "mqttConnectStatus.json requested");

    char statusJSON[100];

    sprintf(statusJSON, "{\"mqtt_connect_status\":%d}", g_mqtt_connect_status);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, statusJSON, strlen(statusJSON));

    return ESP_OK;
}

static esp_err_t http_server_mqtt_disconnect_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "mqttDisconect.json requested");

    mqtt_app_send_message(MQTT_APP_MSG_USER_REQUESTED_DISCONNECT);
    return ESP_OK;
}

static esp_err_t http_server_quantity_json_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "quantity.json requested");

    char recv_buf[MAX_RECV_BUF] = {0};
    int ret = httpd_req_recv(req, recv_buf, MAX_RECV_BUF - 1);
    if (ret <= 0)
    {
        // If no data is received or an error occurred
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    // Parse the JSON data
    cJSON *json = cJSON_Parse(recv_buf);

    if (json == NULL)
    {
        ESP_LOGE(TAG, "Failed to parse JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }
    // Get SSID header
    cJSON *quantity_json = cJSON_GetObjectItemCaseSensitive(json, "quantity");
    if (!cJSON_IsString(quantity_json) || (quantity_json->valuestring == NULL))
    {
        ESP_LOGE(TAG, "quantity not found or not a string");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "quantity not found or not a string");
        return ESP_FAIL;
    }
    quantity = atoi(quantity_json->valuestring);
    save_quantity_to_nvs(quantity);

    // Prepare and send success response
    const char *resp_str = "{\"message\":\"Data received successfully!\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_send(req, resp_str, strlen(resp_str));

    cJSON_Delete(json);
    return ESP_OK;
}

static esp_err_t http_server_favicon_ico_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "favicon.ico requested");

    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);

    return ESP_OK;
}

static httpd_handle_t http_sever_configure(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    xTaskCreatePinnedToCore(&http_server_monitor, "http_server_monitor", 4096, NULL, 3, &task_http_server_monitor, 0);

    http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

    config.core_id = 0;

    // Adjust the default priority to 1 less than the wifi application task
    config.task_priority = 4;

    // Bump up the stack size (default is 4096)
    config.stack_size = 8192;

    // Increase uri handlers
    config.max_uri_handlers = 20;

    // Increase the timeout limits
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    ESP_LOGI(TAG,
             "http_server_configure: Starting server on port: '%d' with task priority: '%d'",
             config.server_port,
             config.task_priority);
    // Start the httpd http_sever_handle

    if (httpd_start(&http_server_handle, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");

        // register jquery.js handler
        httpd_uri_t jquery_js = {
            .uri = "/jquery-3.3.1.min.js",
            .method = HTTP_GET,
            .handler = http_server_jquery_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &jquery_js);

        // register index.html handler
        httpd_uri_t index_html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = http_server_index_html_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &index_html);
        // register app.css handler
        httpd_uri_t app_css = {
            .uri = "/app.css",
            .method = HTTP_GET,
            .handler = http_server_app_css_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &app_css);

        // register app.js handler
        httpd_uri_t app_js = {
            .uri = "/app.js",
            .method = HTTP_GET,
            .handler = http_server_app_js_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &app_js);

        httpd_uri_t favicon_ico = {
            .uri = "/favicon.ico",
            .method = HTTP_GET,
            .handler = http_server_favicon_ico_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &favicon_ico);

        // register wifiConnect.json handler
        httpd_uri_t wifi_connect_json = {
            .uri = "/wifiConnect.json",
            .method = HTTP_POST,
            .handler = http_server_wifi_connect_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &wifi_connect_json);

        // register wifiConnectStatus.json handler
        httpd_uri_t wifi_connect_status_json = {
            .uri = "/wifiConnectStatus",
            .method = HTTP_POST,
            .handler = http_server_wifi_connect_status_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &wifi_connect_status_json);

        // register wifiDisconnect.json handler
        httpd_uri_t wifi_disconnect_json = {
            .uri = "/wifiDisconnect.json",
            .method = HTTP_DELETE,
            .handler = http_server_wifi_disconnect_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &wifi_disconnect_json);
        // register mqttConnect.json handler
        httpd_uri_t mqtt_connect_json = {
            .uri = "/mqttConnect.json",
            .method = HTTP_POST,
            .handler = http_server_mqtt_connect_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &mqtt_connect_json);

        // register mqttConnectStatus.json handler
        httpd_uri_t mqtt_connect_status_json = {
            .uri = "/mqttConnectStatus",
            .method = HTTP_POST,
            .handler = http_server_mqtt_connect_status_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &mqtt_connect_status_json);

        // register mqttDisconnectStatus.json handler
        httpd_uri_t mqtt_disconnect_json = {
            .uri = "/mqttDisconnect.json",
            .method = HTTP_DELETE,
            .handler = http_server_mqtt_disconnect_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &mqtt_disconnect_json);

        httpd_uri_t quantity_jason = {
            .uri = "/quantity.json",
            .method = HTTP_POST,
            .handler = http_server_quantity_json_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &quantity_jason);

        return http_server_handle;
    }
    else
    {
        ESP_LOGI(TAG, "Error starting server!");
    }
    return NULL;
}
BaseType_t http_server_monitor_send_message(http_server_message_e msgID)
{
    http_server_queue_message_t msg;
    msg.msgID = msgID;
    return xQueueSend(http_server_monitor_queue_handle, &msg, portMAX_DELAY);
}
void start_webserver(void)
{
    if (http_server_handle == NULL)
    {
        http_server_handle = http_sever_configure();
    }
}
void stop_webserver(void)
{
    // Stop the httpd server
    if (http_server_handle)
    {
        httpd_stop(http_server_handle);
        ESP_LOGI(TAG, "HTTP SERVER STOP");
        http_server_handle = NULL;
    }
}
void http_set_json_callback(void *cb)
{
    http_get_json_callback = cb;
}
