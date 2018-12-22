#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "esp_log.h"
#include "esp_adc_cal.h"
#include "adc_services.h"

const char *TAG = "adc";

void adc_check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
    {
        ESP_LOGI(TAG, "eFuse Two Point: Supported\n");
    }
    else
    {
        ESP_LOGI(TAG, "eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        ESP_LOGI(TAG, "eFuse Vref: Supported");
    }
    else
    {
        ESP_LOGI(TAG, "eFuse Vref: NOT supported");
    }
}

//(Vin/Vref) * 2^12
// If my Vin = 1.5V then my output will be 1.5/2.5 * 4096 = 2457 in decimal

void adc_read_task(void *arg)
{
    t_adc_config config = *(t_adc_config *) arg;

    adc1_config_width(config.width_bit);
    adc1_config_channel_atten(config.channel, config.atten);

    esp_adc_cal_characteristics_t characteristics;
    esp_adc_cal_characterize(config.adc_num, config.atten, config.width_bit, config.vref, &characteristics);

    while (1)
    {
        uint32_t reading = adc1_get_raw(config.channel);

        uint32_t voltage;
        esp_adc_cal_get_voltage(config.channel, &characteristics, &voltage);
        float prcnt = (float) voltage / 3300 * 100;

        if (voltage == 0)
        {
            vTaskDelete(NULL);
        }

        ESP_LOGI(TAG, "raw: %d,Voltage: \t%d mV, \tPercent: (%.2f)", reading, voltage, prcnt);

        vTaskDelay(100 / portTICK_RATE_MS);
    }
}