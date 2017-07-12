#include <espressif/esp_common.h>
#include <esp8266.h>
#include <esp/uart.h>
#include <stdio.h>
#include <homekit/homekit.h>

#define WIFI_SSID "ANDROT"
#define WIFI_PASS "92@Bz5#3"

void user_init(void) {
    /* Serial Initialization */
    uart_set_baud(0, 115200);
    printf("ESP8266 HOMEKIT - INITIALIZATION\n");
    printf("PAIRING CODE: 121-00-121\n");

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
