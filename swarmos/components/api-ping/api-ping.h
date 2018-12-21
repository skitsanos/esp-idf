static esp_err_t _http_ping_event_handle(esp_http_client_event_t *evt);

void task_http_api_ping(void *pvParameters);
void task_http_device_api_ping(void *pvParameters);