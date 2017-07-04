#include <espressif/esp_common.h>
#include <esp8266.h>
#include <esp/uart.h>
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <ssid_config.h>
#include <lwip/tcp.h>
#include <mdnsresponder.h>

#include "config.h"

static void server_close(struct tcp_pcb *pcb) {
   tcp_arg(pcb, NULL);
   tcp_sent(pcb, NULL);
   tcp_recv(pcb, NULL);
   tcp_close(pcb);

   printf("[TCP] Closing...\n");
}

char* get_value(char* line) {
    char* o = "";
    char* ch = strtok(line, ": ");
    while(ch != NULL) {
        o = ch;
        ch = strtok(NULL, "\r\n");
    }
    return o;
}

uint16_t get_payload_len(char* req) {
    uint16_t payload_len = 0;
    char* sep = strtok(req, "\r\n"); 

    while(sep != NULL) {
        if(!strncmp(sep, "Content-Length: ", 16)) {
            payload_len = (uint16_t)strtol(get_value(sep), (char**)NULL, 10);
        }
        sep = strtok(NULL, "\r\n\r\n"); 
    }
    return payload_len;
}

static err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (err == ERR_OK && p != NULL) {
        tcp_recved(pcb, p->tot_len);
        printf("[TCP] Request received.\n");

        // Parse request metadata.
        char* req = p->payload;
        uint16_t request_len = p->tot_len;
        uint16_t payload_len = get_payload_len(req);
        uint16_t header_len = request_len - payload_len;

        // Extract payload from request.
        char* payload = (char*)malloc(payload_len);
        memcpy(payload, &req[header_len], payload_len);

#if DEBUG_FLAG
        printf("[DEBUG] Header have %d bytes.\n", header_len);
        printf("[DEBUG] Payload have %d bytes.\n", payload_len);
        for(int i=0; i<payload_len; i++) {
            printf("0x%.2x ", payload[i]);
        }
        printf("\n");
#endif

        free(payload);
        pbuf_free(p);
    } else {
        pbuf_free(p);
        server_close(pcb);
    }

    return ERR_OK;
}

static err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err) {
    printf("[TCP] New client connected!\n");

    tcp_recv(pcb, server_recv);
    tcp_accepted(pcb);
    return ERR_OK;
}

void httpd_task(void *pvParameters) {
    struct tcp_pcb *pcb;

    pcb = tcp_new();
    pcb->so_options |= SOF_KEEPALIVE;
    tcp_bind(pcb, IP_ADDR_ANY, IP_ADDRESS);

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, server_accept);
    lwip_init();

    for (;;);
}

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

    /* 
     * Initialize mDNS Discovery 
     * Custom mdnsresponder.h is needed!
     */
    mdns_init();
    mdns_add_facility("ESP8266", "_hap", "", mdns_TCP, IP_ADDRESS, 4500);

    /* Initialize Tasks */
    xTaskCreate(&httpd_task, "HTTP Daemon", 128, NULL, 2, NULL);
}
