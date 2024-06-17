#include <stdio.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "main.h"

#define WIFI_SSID "Fios-p8QKn"
#define WIFI_PASS "4GLAWzdd"

static const char *TAG = "wifi_example";

// Maybe start using ESP_ERROR_CHECK macro for wifi library functions

static void wifi_sta_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START){
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Attempting to reconnect...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        xTaskCreate(&http_post_task, "http_post_task", 8192, NULL, 5, NULL);
    }
}

void wifi_init_sta(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "connecting to ap SSID: %s", WIFI_SSID);
}

void http_post_task(void *pvParameters)
{
    printf("HANDLING POST TASK\n");
    esp_http_client_config_t config = {
        .url = "http://192.168.1.171:5000/upload",
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    const char *post_data = "Hello from ESP32";
    esp_http_client_set_url(client, config.url);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);

    if(err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %ld",
                 (int)esp_http_client_get_status_code(client),
                 (long int)esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}

void app_main(void)
{
    printf("hello world\n");

    ESP_ERROR_CHECK(nvs_flash_init());

    esp_event_loop_create_default();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_sta_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_sta_event_handler, NULL, NULL);

    wifi_init_sta();
}