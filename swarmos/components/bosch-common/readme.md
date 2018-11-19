# Bosch common functions syntax

### bosch_i2c_master_init

Initializes I2C driver so you can perform operations on it.

Parameters accepted:

- I2C mode;
- SDA pin number;
- SCL pin number;

Returns

 - esp_err_t, - _ESP_OK_ if init went well, otherwise error code
 
 Example:

```c
bosch_i2c_master_init(I2C_MODE_MASTER, GPIO_NUM_4, GPIO_NUM_5);
```
