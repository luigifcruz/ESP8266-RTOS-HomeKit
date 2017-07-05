#include <espressif/esp_common.h>
#include <mdnsresponder.h>
#include <lwip/tcp.h>
#include <esp8266.h>
#include <esp/uart.h>
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "tcp.h"
#include "enum.h"
#include "pairing.h"
#include "crypto/crypto.h"
#include "crypto/tlv.h"
#include "homekit.h"

void handle_request(uint8_t* payload, uint16_t p_lenght, struct tcp_pcb *pcb) {
    uint8_t type; 
    uint16_t length;
    uint8_t* value;

    while (tlv_decode_next(&payload, &p_lenght, &type, &length, &value)) {
        printf("[TLV] Tag received: ");
        switch(type) {
            case TYPE_STATE:
                printf("Pairing process ");
                switch(value[0]) {
                    case PAIR_M1: handle_M1(pcb); break;
                    case PAIR_M3: handle_M3(pcb); break;
                }
                break;
            case TYPE_METHOD:
                printf("Pairing method.\n");
                break;
            case TYPE_IDENTIFIER:
                printf("Authentication.\n");
                break;
            default:
                printf("Not recognized (%d).\n", type);
        }
    }
}

void homekit_init() {
    /* Crpyto Initialization (requires a shitload of memory) */
    printf("Crypto Initialization: This should take 10-30 seconds.\n");
    crypto_init();

    /* 
     * mDNS Initialization 
     * Custom mdnsresponder.h is needed!
     */
    mdns_init();
    mdns_add_facility("ESP8266", "_hap", "", mdns_TCP, IP_ADDRESS, 4500);

    /* Tasks Initialization */
    xTaskCreate(&tcp_task_init, "TCP Daemon", 128, NULL, 2, NULL);
}