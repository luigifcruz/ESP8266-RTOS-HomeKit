#include <espressif/esp_common.h>
#include <esp8266.h>
#include <esp/uart.h>
#include <stdio.h>
#include <homekit/homekit.h>

#define WIFI_SSID "WIFI_NAME"
#define WIFI_PASS "WIFI_PASS"

void user_init(void) {
    /* Serial Initialization */
    uart_set_baud(0, 115200);
    printf("ESP8266 HOMEKIT - INITIALIZATION\n");

    /* Let's (safely) overclock */
    sdk_system_update_cpu_freq(160);

    /* Connect to a WiFi */
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);
    sdk_wifi_station_connect();

    /* HomeKit Initialization */
    homekit_init();
}
