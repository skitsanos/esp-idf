/**
 * Bosch BME 680 Sensor module wrapper
 * Sensor operation at full accuracy:
 * Pressure:     300...1100 hPa
 * Humidity:     0…100%
 * Temperature:  -40…85°C
 *
 * @author skitsanos
 * @version 1.0
 */

#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#include "bme680_selftest.h"
#include "bme680.h"
#include "bme680_defs.h"

#define TAG "bme680"

#define TASK_STACK_DEPTH 2048

#define MIN_TEMPERATURE INT16_C(0)        /* 0 degree Celsius */
#define MAX_TEMPERATURE INT16_C(6000)    /* 60 degree Celsius */

#define MIN_PRESSURE UINT32_C(90000)    /* 900 hecto Pascals */
#define MAX_PRESSURE UINT32_C(110000)    /* 1100 hecto Pascals */

#define MIN_HUMIDITY UINT32_C(20000)    /* 20% relative humidity */
#define MAX_HUMIDITY UINT32_C(90000)    /* 90% relative humidity*/

#define HEATR_DUR    2000
#define N_MEAS        6
#define LOW_TEMP    150
#define HIGH_TEMP    350

#define BME_SDA_PIN GPIO_NUM_4
#define BME_SCL_PIN GPIO_NUM_5

#define BME_INIT_VALUE (0)

#define SUCCESS ((uint8_t)0)
#define ERROR ((int8_t)-1)

extern int8_t BME_I2C_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);

int8_t BME_I2C_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt);

/*
 * Function to analyze the sensor data
 */

static int8_t analyze_sensor_data(struct bme680_field_data *data, uint8_t n_meas)
{
    int8_t rslt = BME680_OK;
    uint8_t self_test_failed = 0, i;
    uint32_t cent_res = 0;

    if ((data[0].temperature < MIN_TEMPERATURE) || (data[0].temperature > MAX_TEMPERATURE))
        self_test_failed++;

    if ((data[0].pressure < MIN_PRESSURE) || (data[0].pressure > MAX_PRESSURE))
        self_test_failed++;

    if ((data[0].humidity < MIN_HUMIDITY) || (data[0].humidity > MAX_HUMIDITY))
        self_test_failed++;

    for (i = 0; i < n_meas; i++) /* Every gas measurement should be valid */
        if (!(data[i].status & BME680_GASM_VALID_MSK))
            self_test_failed++;

    if (n_meas >= 6)
        cent_res = (data[3].gas_resistance + data[5].gas_resistance) / (2 * data[4].gas_resistance);

    if ((cent_res * 5) < 6)
        self_test_failed++;

    if (self_test_failed)
        rslt = BME680_W_SELF_TEST_FAILED;

    return rslt;
}

int8_t bme680_self_test(struct bme680_dev *dev)
{
    int8_t rslt = BME680_OK;
    struct bme680_field_data data[N_MEAS];

    struct bme680_dev t_dev;

    /* Copy required parameters from reference bme680_dev struct */
    t_dev.dev_id = dev->dev_id;
    t_dev.amb_temp = 25;
    t_dev.read = dev->read;
    t_dev.write = dev->write;
    t_dev.intf = dev->intf;
    t_dev.delay_ms = dev->delay_ms;

    rslt = bme680_init(&t_dev);

    if (rslt == BME680_OK)
    {
        /* Select the power mode */
        /* Must be set before writing the sensor configuration */
        t_dev.power_mode = BME680_FORCED_MODE;

        uint16_t settings_sel;

        /* Set the temperature, pressure and humidity & filter settings */
        t_dev.tph_sett.os_hum = BME680_OS_1X;
        t_dev.tph_sett.os_pres = BME680_OS_16X;
        t_dev.tph_sett.os_temp = BME680_OS_2X;

        /* Set the remaining gas sensor settings and link the heating profile */
        t_dev.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
        t_dev.gas_sett.heatr_dur = HEATR_DUR;

        settings_sel = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_GAS_SENSOR_SEL;

        uint16_t profile_dur = 0;
        bme680_get_profile_dur(&profile_dur, &t_dev);

        uint8_t i = 0;
        while ((rslt == BME680_OK) && (i < N_MEAS))
        {
            if (rslt == BME680_OK)
            {

                if (i % 2 == 0)
                    t_dev.gas_sett.heatr_temp = HIGH_TEMP; /* Higher temperature */
                else
                    t_dev.gas_sett.heatr_temp = LOW_TEMP; /* Lower temperature */

                rslt = bme680_set_sensor_settings(settings_sel, &t_dev);

                if (rslt == BME680_OK)
                {

                    rslt = bme680_set_sensor_mode(&t_dev); /* Trigger a measurement */

                    t_dev.delay_ms(profile_dur); /* Wait for the measurement to complete */

                    rslt = bme680_get_sensor_data(&data[i], &t_dev);
                }
            }

            i++;
        }

        if (rslt == BME680_OK)
            rslt = analyze_sensor_data(data, N_MEAS);
    }

    return rslt;
}

static void bme680_delay_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}


/*
 * Read BME680 sensors data
 */
void task_bme680_read()
{
    struct bme680_dev dev = {
            .dev_id = BME680_I2C_ADDR_SECONDARY,
            .intf =   BME680_I2C_INTF,
            .read =   (bme680_com_fptr_t *) BME_I2C_bus_read,
            .write =  (bme680_com_fptr_t *) BME_I2C_bus_write,
            .delay_ms = bme680_delay_ms,
            .amb_temp = 25
    };

    int8_t rslt = BME680_OK;
    rslt = bme680_init(&dev);
    if (rslt == BME680_OK)
    {

    }
    //bme680_self_test(dev;)

    vTaskDelete(NULL);
}