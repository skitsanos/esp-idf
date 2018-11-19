#include <stdlib.h>

#include "esp_log.h"
#include "esp_system.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#define BME_INIT_VALUE (0)

#define SUCCESS ((uint8_t)0)
#define ERROR ((int8_t)-1)

#define TAG "bosch-i2c"

esp_err_t bosch_i2c_master_init(i2c_mode_t mode, gpio_num_t sda_io_num, gpio_num_t scl_io_num)
{
    i2c_config_t i2c_config = {
            .mode = mode,
            .sda_io_num = sda_io_num,
            .scl_io_num = scl_io_num,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = 1000000
    };
    i2c_param_config(I2C_NUM_0, &i2c_config);

    esp_err_t ret = i2c_driver_install(I2C_NUM_0, mode, 0, 0, 0);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "I2C driver installed. SDA:%d, SCL:%d", sda_io_num, scl_io_num);
    }
    else
    {
        ESP_LOGI(TAG, "I2C driver failed. Err code: %d", ret);
    }

    return ret;
}

int8_t BME_I2C_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
    int32_t iError = BME_INIT_VALUE;

    esp_err_t espRc;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    if (i2c_master_start(cmd) == ESP_OK)
    {
        ESP_LOGW(TAG, "i2c_master_write_byte: %d",
                 i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true));
        ESP_LOGW(TAG, "i2c_master_write_byte: %d", i2c_master_write_byte(cmd, reg_addr, true));
        ESP_LOGW(TAG, "i2c_master_write: %d", i2c_master_write(cmd, reg_data, cnt, true));
        ESP_LOGW(TAG, "i2c_master_stop: %d", i2c_master_stop(cmd));

        espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
        if (espRc == ESP_OK)
        {
            iError = SUCCESS;
        }
        else
        {
            ESP_LOGW(TAG, "Failed at BME_I2C_bus_write");
            iError = FAIL;
        }
        i2c_cmd_link_delete(cmd);
    }
    else
    {
        ESP_LOGW(TAG, "Failed on i2c_master_start");
        iError = FAIL;
    }

    return (int8_t) iError;
}

int8_t BME_I2C_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
    int32_t iError = BME_INIT_VALUE;
    esp_err_t espRc;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

    if (cnt > 1)
    {
        i2c_master_read(cmd, reg_data, (size_t) (cnt - 1), I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, reg_data + cnt - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    if (espRc == ESP_OK)
    {
        iError = SUCCESS;
    }
    else
    {
        ESP_LOGW(TAG, "Failed at BME_I2C_bus_read");
        iError = FAIL;
    }

    i2c_cmd_link_delete(cmd);

    return (int8_t) iError;
}