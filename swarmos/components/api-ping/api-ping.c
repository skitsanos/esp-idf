#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_http_client.h"

#include "../../main/app_config.h"
#include "../../main/app.h"

#include "../wifi/wifi_bits.h"

#include "../http/http_services.h"

#include "cJSON.h"
#include "../utils/str_builder.h"

extern uint8_t temprature_sens_read();

extern uint32_t hall_sens_read();

static esp_err_t _http_ping_event_handle(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char *) evt->data);
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*if (!esp_http_client_is_chunked_response(evt->client))
            {
                printf("%.*s", evt->data_len, (char *) evt->data);
            }*/
            ESP_LOGI(TAG, "%.*s", evt->data_len, (char *) evt->data);
            char *_content_type;
            esp_http_client_get_header(evt->client, "Content-Type", &_content_type);
            ESP_LOGI(TAG, "%s", _content_type);

            if (strcmp(_content_type, "application/json") == 0)
            {
                cJSON *root = cJSON_Parse(evt->data);
                cJSON *_json_response_type = cJSON_GetObjectItem(root, "type");
                if (_json_response_type != NULL)
                {
                    ESP_LOGI(TAG, "API response type: %s", _json_response_type->valuestring);
                }
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

void task_http_api_ping(void *pvParameters)
{
    t_http_request_parameters parameters = {
            .protocol = "http",
            .server = "api.skitsanos.com",
            .port = 80,
            .url = "/",
            .content_type = "application/json",
            .query_string = NULL,
            .event_handler = _http_ping_event_handle
    };

    t_http_request_parameters *p = malloc(sizeof(t_http_request_parameters));
    memcpy(p, &parameters, sizeof(t_http_request_parameters));

    ESP_LOGI(TAG, "Connecting to %s...", p->server);
    xTaskCreate(task_http_get, "http get ", 2048, p, 10, NULL);

    vTaskDelete(NULL);
}

void task_http_device_api_ping(void *pvParameters)
{
    uint8_t temp = temprature_sens_read();
    uint32_t hall_sensor = hall_sens_read();

    float celsius = (float) ((temp - 32) / 1.8);
    char celsius_string[sizeof(float)];
    sprintf(celsius_string, "%.2f", celsius);

    str_builder_t *sb = str_builder_create();
    str_builder_add_str(sb, "/?core_temp=", 0);
    str_builder_add_str(sb, celsius_string, 0);
    str_builder_add_str(sb, "&hall=", 0);
    str_builder_add_int(sb, hall_sensor);

    char *buffer = str_builder_dump(sb, NULL);

    t_http_request_parameters parameters = {
            "http",
            "api.skitsanos.com",
            80,
            buffer,
            "application/json",
            NULL,
            _http_ping_event_handle
    };

    str_builder_destroy(sb);

    t_http_request_parameters *p = malloc(sizeof(t_http_request_parameters));
    memcpy(p, &parameters, sizeof(t_http_request_parameters));

    ESP_LOGI(TAG, "Connecting on %s at %s", p->server, p->url);
    xTaskCreate(task_http_get, "http get ", 2048, p, 10, NULL);

    vTaskDelete(NULL);
}
