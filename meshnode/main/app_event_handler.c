/**
 * Application Event handler
 * @author skitsanos
 */
#include "esp_wifi.h"
#include "esp_wps.h"
#include "esp_log.h"
#include "esp_system.h"

#include "esp_event.h"

#include "app_config.h"

char *wifiReason(uint8_t reason);

static const char *TAG = "loggerdaisy";

static esp_err_t eventHandler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
        case SYSTEM_EVENT_STA_START: //https://dl.espressif.com/doc/esp-idf/latest/api-guides/wifi.html
            ESP_LOGI(TAG, "WiFi STA started");
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "Got STA IP address");
            ESP_LOGI(TAG, "sta ip: "
                    IPSTR
                    ", mask: "
                    IPSTR
                    ", gw: "
                    IPSTR,
                     IP2STR(&event->event_info.got_ip.ip_info.ip),
                     IP2STR(&event->event_info.got_ip.ip_info.netmask),
                     IP2STR(&event->event_info.got_ip.ip_info.gw));
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGD(TAG, "SYSTEM_EVENT_STA_DISCONNECTED, ssid:%s, ssid_len:%d, bssid:"
                    MACSTR
                    ", reason:%s",
                     event->event_info.disconnected.ssid,
                     event->event_info.disconnected.ssid_len,
                     MAC2STR(event->event_info.disconnected.bssid),
                     wifiReason(event->event_info.disconnected.reason));
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;

        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            /*point: the function esp_wifi_wps_start() only get ssid & password
             * so call the function esp_wifi_connect() here
             * */
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
            ESP_ERROR_CHECK(esp_wifi_wps_disable());
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;

        case SYSTEM_EVENT_AP_START:
            ESP_LOGI(TAG, "WiFi AP started");
            break;

        case SYSTEM_EVENT_AP_STOP:
            ESP_LOGI(TAG, "WiFi AP stopped");
            break;

        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "station:"
                    MACSTR
                    " join, AID=%d",
                     MAC2STR(event->event_info.sta_connected.mac),
                     event->event_info.sta_connected.aid);
            break;

        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "station:"
                    MACSTR
                    "leave, AID=%d",
                     MAC2STR(event->event_info.sta_disconnected.mac),
                     event->event_info.sta_disconnected.aid);
            break;

        default:
            break;
    }
    return ESP_OK;
}

char *wifiReason(uint8_t reason)
{
    char *result = "UNKNOWN";

    switch (reason)
    {
        case WIFI_REASON_UNSPECIFIED:
            result = "WIFI_REASON_UNSPECIFIED";
            break;

        case WIFI_REASON_AUTH_EXPIRE:
            result = "WIFI_REASON_AUTH_EXPIRE";
            break;

        case WIFI_REASON_AUTH_LEAVE:
            result = "WIFI_REASON_AUTH_LEAVE";
            break;

        case WIFI_REASON_ASSOC_EXPIRE:
            result = "WIFI_REASON_ASSOC_EXPIRE";
            break;

        case WIFI_REASON_ASSOC_TOOMANY:
            result = "WIFI_REASON_ASSOC_TOOMANY";
            break;

        case WIFI_REASON_NOT_AUTHED:
            result = "WIFI_REASON_NOT_AUTHED";
            break;

        case WIFI_REASON_ASSOC_LEAVE:
            result = "WIFI_REASON_ASSOC_LEAVE";
            break;

        case WIFI_REASON_ASSOC_NOT_AUTHED:
            result = "WIFI_REASON_ASSOC_NOT_AUTHED";
            break;

        case WIFI_REASON_DISASSOC_PWRCAP_BAD:
            result = "WIFI_REASON_DISASSOC_PWRCAP_BAD";
            break;

        case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
            result = "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
            break;

        case WIFI_REASON_IE_INVALID:
            result = "WIFI_REASON_IE_INVALID";
            break;

        case WIFI_REASON_MIC_FAILURE:
            result = "WIFI_REASON_MIC_FAILURE";
            break;

        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            result = "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
            break;

        default:
            break;
    }

    return result;
}