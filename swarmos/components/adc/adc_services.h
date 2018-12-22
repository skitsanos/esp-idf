#include "esp_adc_cal.h"

typedef struct
{
    uint32_t vref;
    adc1_channel_t channel;
    adc_bits_width_t width_bit;
    adc_atten_t atten;
    adc_unit_t adc_num;
} t_adc_config;

void adc_check_efuse();

void adc_read_task(void *arg);