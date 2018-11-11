# swarm-os

|   Last build    | v.1.0.20181109, ld-node 3         |
|----------------:|:----------------------------------|
| ESP-IDF version |ESP-IDF v3.2-dev-1175-g11b444b8f   |
| Tested boards   |Doit ESP32 Devkit 1.0 (ESP32D0WDQ6)|
||[Wemos ESP32+OLED](https://www.aliexpress.com/item/Lolin-ESP32-OLED-wemos-for-Arduino-ESP32-OLED-WiFi-Modules-Bluetooth-Dual-ESP-32-ESP-32S/32807531243.html)|
||[Wemos](https://www.aliexpress.com/item/ESP32-wemos-ESP32-WiFi-Modules-Bluetooth-Dual-ESP-32-ESP-32S-ESP8266/32817523724.html)|
||[Le Xin ESP32](https://www.aliexpress.com/item/Lolin-ESP32-OLED-V2-0-Pro-ESP32-OLED-wemos-pour-Arduino-ESP32-OLED-WiFi-Modules-Bluetooth/32824819112.html)
||Widora Air v.5|
||[TTGO ESP32+18650](https://www.aliexpress.com/item/TTGO-WiFi-Bluetooth-Module-18650-Battery-Holder-Seat-2A-Fuse-ESP32-4MB-SPI-Flash-Oct30-Drop/32951284716.html)|
||[TTGO ESP32+Lora+GPS](https://www.aliexpress.com/item/For-T-Beam-ESP32-868-915Mhz-WiFi-Wireless-Bluetooth-Module-GPS-NEO-6M-SMA-LORA-Board/32951848698.html)|
||[TTGO T7 ESP32 Pico-D4](https://www.aliexpress.com/item/TTGO-T7-ESP32-Module-PICO-D4-4MB-SPI-Flash-development-board/32841749492.html7)|
||[ESP32-CAM WiFi](https://www.aliexpress.com/item/ESP32-CAM-WiFi-Bluetooth-Module-Camera-Module-Development-Board-ESP32-with-Camera-Module-OV2640-2MP/32919183232.html)|
||[TTGO Mini32 v.2.0](https://www.aliexpress.com/item/TTGO-MINI32-V2-0-ESP32-rev1-rev-one-WiFi-Bluetooth-Module-For-D1-mini/32845357819.html)|

### Networking

- [x] wifi AP mode
- [x] wifi STA mode
- [x] wifi APSTA mode
- [x] wifi esp-now

- [ ] ntp services
  - [x] ntp_check_time
  - [x] ntp_get_datetime
  - [x] task_get_datetime
- [ ] dns
  - [x] dns_check

- [x] http
  - [x] task_http_get
  - [x] task_http_device_api_ping

### Screen

- [x] screen_on
- [x] screen_off
- [x] screen_inverted
- [x] screen_set_contrast
- [x] screen_clear
- [x] screen_update
- [x] screen_flip_h
- [x] screen_flip_v
- [x] screen_draw_pixel
- [x] screen_set_default_font
- [x] screen_set_font
- [x] screen_print_centered
- [x] screen_print_anchored
- [ ] screen_print_scrolled
- [x] screen_print_at_pos
- [x] screen_draw_line
- [x] screen_draw_line_h
- [x] screen_draw_line_v
- [x] screen_draw_rect
- [x] screen_draw_rect_filled
- [x] screen_draw_circle_quads
- [x] screen_draw_circle
- [x] screen_draw_circle_filled
- [x] screen_draw_rect_rounded
- [x] screen_draw_progressbar
- [x] screen_draw_network_icon
- [ ] screen_scroll_v
- [x] task_screen_init

### Sensors

- [x] Core
  - [x] task_read_internal_sensors
- [ ] Bosch
  - [x] BME-280
  - [ ] BME-680
