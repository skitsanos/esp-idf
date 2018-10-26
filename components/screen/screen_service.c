/**
 *
 */
#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "ssd1306.h"
#include "font.h"

#include "iface_esp32_i2c.h"
#include "iface_virtual.h"

#include "../../main/app_config.h"

#define SDA_PIN GPIO_NUM_5
#define SCL_PIN GPIO_NUM_4

#define SCREEN_INITIALIZED_BIT BIT0

struct SSD1306_Device Dev_I2C;

//Check if screen I2C port and screen itself is initialized
bool screen_initialized()
{
    if (ESP32_InitI2CMaster(SDA_PIN, SCL_PIN))
    {
        return (SSD1306_Init_I2C(&Dev_I2C, 128, 64, 0x3C, 0, ESP32_WriteCommand_I2C, ESP32_WriteData_I2C, NULL) == 1)
               ? true
               : false;
    }
    else
    {
        return false;
    }
}

//Enable or disable display RAM
void screen_ram(bool enable)
{
    if (enable == true)
    {
        SSD1306_EnableDisplayRAM(&Dev_I2C);
    }
    else
    {
        SSD1306_DisableDisplayRAM(&Dev_I2C);
    }
}

//Update the screen
void screen_update()
{
    SSD1306_Update(&Dev_I2C);
}

//Clear the screen
void screen_clear()
{
    SSD1306_Clear(&Dev_I2C, false);
}

//Set default font for the screen
void screen_set_default_font()
{
    SSD1306_SetFont(&Dev_I2C, &Font_Segoe_UI_11x11);
}

//Set custom screen font
void screen_set_font(struct FontDef *FontHandle)
{
    SSD1306_SetFont(&Dev_I2C, FontHandle);
}

//Set screen inversion
void screen_inverted(bool Inverted)
{
    SSD1306_SetInverted(&Dev_I2C, Inverted);
}

//Flip the screen horizontally
void screen_flip_h(bool On)
{
    SSD1306_SetHFlip(&Dev_I2C, On);
}

//Flip the screen vertically
void screen_flip_v(bool On)
{
    SSD1306_SetVFlip(&Dev_I2C, On);
}

//Print text in the middle of the screen
void screen_print_centered(const char *Text)
{
    FontDrawAnchoredString(&Dev_I2C, Text, TextAnchor_Center, true);
}

//Print text at the given position
void screen_print_at_pos(int x, int y, const char *Text)
{
    //screen_set_default_font();
    FontDrawStringUnaligned(&Dev_I2C, Text, x, y, true);
}

void task_screen_init(void *pvParameters)
{
    /* for (;;)
     {

     }*/

    /* Tasks must not attempt to return from their implementing
    function or otherwise exit.  In newer FreeRTOS port
    attempting to do so will result in an configASSERT() being
    called if it is defined.  If it is necessary for a task to
    exit then have the task call vTaskDelete( NULL ) to ensure
    its exit is clean. */

    if (screen_initialized())
    {
        screen_inverted(true);
        screen_flip_v(true);
        screen_clear();
        screen_set_default_font();

        screen_print_centered("LoggerDaisy. Welcome!");

        screen_print_at_pos(0, 0, "Booting...");

        screen_update();
    }

    vTaskDelete(NULL);
}
