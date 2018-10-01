/**
 * Reading internal ESP32 sensors (temperature and hall)
 * @author skitsanos
 * @version 1.0
 */
#include <stdlib.h>
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG_SYSTEM = "system";

extern uint8_t temprature_sens_read();
extern uint32_t hall_sens_read();

void task_read_internal_sensors(void *ignore)
{
    uint8_t temp = temprature_sens_read();
    uint32_t hall_sensor = hall_sens_read();

    float celsius = (float) ((temp - 32) / 1.8);

    ESP_LOGI(TAG_SYSTEM, "%.2f degC", celsius);
    ESP_LOGI(TAG_SYSTEM, "%d", hall_sensor);
    vTaskDelete(NULL);
}
