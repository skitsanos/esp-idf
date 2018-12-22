### Using adc_services in _swarmos/ldnode_ example

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#include "adc_services.h"

#define TAG "soil-moisture"

#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_7) //pin D35 on ESP32
#define V_REF 1000  // ADC reference voltage

void app_main()
{
    ESP_LOGI(TAG, "Loading...");

    adc_check_efuse();

    esp_err_t status;
    status = adc2_vref_to_gpio(GPIO_NUM_25);
    if (status == ESP_OK)
    {
        printf("v_ref routed to GPIO\n");
    }
    else
    {
        printf("failed to route v_ref\n");
    }

    t_adc_config config = {
            .vref = V_REF,
            .atten = ADC_ATTEN_11db,
            .channel = ADC1_TEST_CHANNEL,
            .width_bit = ADC_WIDTH_12Bit,
            .adc_num = ADC_UNIT_1
    };

    t_adc_config *p = malloc(sizeof(t_adc_config));
    memcpy(p, &config, sizeof(t_adc_config));

    xTaskCreate(adc_read_task, "ADC read task", 2048, p, 5, NULL);
}
```
