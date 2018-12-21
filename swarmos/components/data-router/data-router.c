#include <stdarg.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "data-router.h"

#define TAG "data-router"

/**
 * Sends data into selected channel.
 * In order to not block the OS, each packet sending executed as task
 *
 * @param ch channel type
 * @param config channel configuration
 * @param data data to be sent
 * @return ESP_OK if succeeded or ESP_FAIL if an error occured
 */
esp_err_t data_router_send(channels ch, void *config, ...)
{
    esp_err_t ret = ESP_OK;

    va_list args;
    va_start(args, config);
    //vprintf(config, args);


    switch (ch)
    {
        case ROUTER_CHANNEL_CONSOLE:
        {
            vprintf(config, args);

            t_data_router_config_console cfg;
            cfg.format = config;
            cfg.data = args;

            t_data_router_config_console *p = malloc(sizeof(t_data_router_config_console));
            memcpy(p, &cfg, sizeof(t_data_router_config_console));

            xTaskCreate(task_data_router_console, "route data to console", 2048, p, 10, NULL);

            ret = ESP_OK;
            break;
        }

        case ROUTER_CHANNEL_ESP_NOW:
        {
            ret = ESP_OK;
            break;
        }

        case ROUTER_CHANNEL_HTTP:
        {
            ret = ESP_OK;
            break;
        }

        default:
            ret = ESP_FAIL;
            break;
    }

    va_end(args);
    return ret;
}

void task_data_router_console(void *params)
{
    t_data_router_config_console cfg = *(t_data_router_config_console *) params;
    printf(cfg.format, cfg.data);
    
    vTaskDelete(NULL);
}

void task_data_router_http(void *params)
{}