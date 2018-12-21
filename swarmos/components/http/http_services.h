#include "esp_http_client.h"

typedef struct
{
    char *protocol;
    char *server;
    int port;
    char *url;
    char *content_type;
    esp_http_client_method_t method;
    char *query_string;
    http_event_handle_cb event_handler;
} t_http_request_parameters;

void task_http_get(void *pvParameters);
void task_http_execute(void *pvParameters);

esp_err_t _http_event_handler(esp_http_client_event_t *evt);