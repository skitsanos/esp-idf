/*
 * @author skitsanos
 */

#include "esp_system.h"

typedef enum
{
    ROUTER_CHANNEL_CONSOLE = 0,
    ROUTER_CHANNEL_UART = 1,
    ROUTER_CHANNEL_ESP_NOW = 2,
    ROUTER_CHANNEL_HTTP = 3,
    ROUTER_CHANNEL_MQTT = 4,
    ROUTER_CHANNEL_SOCKET = 5
} channels;

typedef struct
{
    char *format;
    __VALIST data;
} t_data_router_config_console;

void task_data_router_console(void *params);

esp_err_t data_router_send(channels ch, void *config, ...);