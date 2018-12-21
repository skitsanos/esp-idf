/**
 * Reading internal ESP32 sensors (temperature and hall)
 * @author skitsanos
 * @version 1.0
 */
#include <stdlib.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "../utils/str_builder.h"

static const char *TAG_SYSTEM = "system";

extern uint8_t temprature_sens_read();

extern uint32_t hall_sens_read();

void task_read_internal_sensors(void *ignore)
{
    uint8_t temp = temprature_sens_read();
    uint32_t hall_sensor = hall_sens_read();

    float celsius = (float) ((temp - 32) / 1.8);

    ESP_LOGI(TAG_SYSTEM, "Core temperature sensor: %.2f degC", celsius);
    ESP_LOGI(TAG_SYSTEM, "Core hall sensor: %d", hall_sensor);

    vTaskDelete(NULL);
}

char *esp_deviceMac()
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

float esp_deviceTemperature()
{
    uint8_t temp = temprature_sens_read();

    float celsius = (float) ((temp - 32) / 1.8);
    return celsius;
}

uint32_t esp_deviceHall()
{
    return hall_sens_read();
}