/*
 * HTTP Services
 * @author skitsanos
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

#include "../../main/app_config.h"
#include "../../main/app.h"

#include "../utils/str_builder.h"

#include "../wifi/wifi_bits.h"
#include "../wifi/wifi_services.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_http_client.h"

#include "http_services.h"

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
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

static char *http_user_agent_header_content()
{
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    str_builder_t *sb = str_builder_create();

    str_builder_add_str(sb, "ESP32 (", 0);
    str_builder_add_int(sb, chip_info.cores);
    str_builder_add_str(sb, " cores, WIFI", 0);
    str_builder_add_str(sb, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "", 0);
    str_builder_add_str(sb, (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "", 0);
    str_builder_add_str(sb, ", silicon revision ", 0);
    str_builder_add_int(sb, chip_info.revision);
    str_builder_add_str(sb, "), swarmos, v1.0.20181106", 0);

    char *buffer = str_builder_dump(sb, NULL);
    str_builder_destroy(sb);

    return buffer;
}

/*
 * Device-ID is a MAC address
 */
static char *http_deviceId_header_content()
{
    uint8_t sta_mac[6];
    esp_efuse_mac_get_default(sta_mac);

    str_builder_t *sb = str_builder_create();

    char _temp[6];
    sprintf(_temp, "%x%x%x%x%x%x", sta_mac[0], sta_mac[1], sta_mac[2], sta_mac[3], sta_mac[4], sta_mac[5]);

    str_builder_add_str(sb, _temp, 0);

    char *buffer = str_builder_dump(sb, NULL);
    str_builder_destroy(sb);

    return buffer;
}

void task_http_get(void *pvParameters)
{
    wifi_wait_for_connection();

    //check if actual server exists
    t_http_request_parameters p = *(t_http_request_parameters *) pvParameters;
    ESP_LOGI(TAG, "Connecting to API: %s://%s:%d%s", p.protocol, p.server, p.port, p.url);

    struct addrinfo *res;
    int err = getaddrinfo(p.server, NULL, NULL, &res);

    if (err != 0 || res == NULL)
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
    }
    else
    {
        struct in_addr *addr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        //construct an url
        str_builder_t *sb = str_builder_create();

        str_builder_add_str(sb, p.protocol, 0);
        str_builder_add_str(sb, "://", 0);
        str_builder_add_str(sb, p.server, 0);

        if (p.port != 80) //when port is 80, we don't need to add it into our URL
        {
            str_builder_add_str(sb, ":", 0);
            str_builder_add_int(sb, p.port);
        }

        str_builder_add_str(sb, p.url, 0);

        if (p.query_string != NULL) //add query string if we have any
        {
            str_builder_add_str(sb, "?", 0);
            str_builder_add_str(sb, p.query_string, 0);
        }

        char *buffer = str_builder_dump(sb, NULL);
        str_builder_destroy(sb);

        esp_http_client_config_t config = {
                .url = buffer,
                .event_handler = p.event_handler == NULL ? _http_event_handler : p.event_handler
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "User-Agent", http_user_agent_header_content());
        esp_http_client_set_header(client, "X-Device-ID", http_deviceId_header_content());
        esp_http_client_set_header(client, "Content-Type", p.content_type == NULL ? "text/html" : p.content_type);

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "Status = %d, content_length = %d",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));
        }
        esp_http_client_cleanup(client);

    }

    vTaskDelete(NULL);
}

void task_http_execute(void *pvParameters)
{
    wifi_wait_for_connection();

    //check if actual server exists
    t_http_request_parameters p = *(t_http_request_parameters *) pvParameters;
    ESP_LOGI(TAG, "Connecting to API: %s://%s:%d%s", p.protocol, p.server, p.port, p.url);

    struct addrinfo *res;
    int err = getaddrinfo(p.server, NULL, NULL, &res);

    if (err != 0 || res == NULL)
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
    }
    else
    {
        struct in_addr *addr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        //construct an url
        str_builder_t *sb = str_builder_create();

        str_builder_add_str(sb, p.protocol, 0);
        str_builder_add_str(sb, "://", 0);
        str_builder_add_str(sb, p.server, 0);

        if (p.port != 80) //when port is 80, we don't need to add it into our URL
        {
            str_builder_add_str(sb, ":", 0);
            str_builder_add_int(sb, p.port);
        }

        str_builder_add_str(sb, p.url, 0);

        if (p.query_string != NULL) //add query string if we have any
        {
            str_builder_add_str(sb, "?", 0);
            str_builder_add_str(sb, p.query_string, 0);
        }

        char *buffer = str_builder_dump(sb, NULL);
        str_builder_destroy(sb);

        esp_http_client_config_t config = {
                .url = buffer,
                .event_handler = p.event_handler == NULL ? _http_event_handler : p.event_handler
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "User-Agent", http_user_agent_header_content());
        esp_http_client_set_header(client, "X-Device-ID", http_deviceId_header_content());
        esp_http_client_set_header(client, "Content-Type", p.content_type == NULL ? "text/html" : p.content_type);

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "Status = %d, content_length = %d",
                     esp_http_client_get_status_code(client),
                     esp_http_client_get_content_length(client));
        }
        esp_http_client_cleanup(client);

    }

    vTaskDelete(NULL);
}