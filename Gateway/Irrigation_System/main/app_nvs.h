
#ifndef APP_NVS_H_
#define APP_NVS_H_

/**
 * Saves station mode Wifi credentials to NVS
 * @return ESP_OK if successful.
 */
esp_err_t app_nvs_save_sta_creds(void);

/**
 * Loads the previously saved credentials from NVS.
 * @return true if previously saved credentials were found.
 */
bool app_nvs_load_sta_creds(void);

/**
 * Clears station mode credentials from NVS
 * @return ESP_OK if successful.
 */
esp_err_t app_nvs_clear_sta_creds(void);

esp_err_t app_nvs_clear_mqtt_creds(void);
bool app_nvs_load_mqtt_creds(void);
esp_err_t app_nvs_save_mqtt_creds(void);
esp_err_t save_quantity_to_nvs(int quantity);
bool get_quantity_from_nvs(int *quantity);

#endif /* MAIN_APP_NVS_H_ */