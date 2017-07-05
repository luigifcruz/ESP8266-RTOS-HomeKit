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

#include <homekit/tlv.h>
#include <homekit/crypto.h>
#include "config.h"

uint8_t pairing_state = 0x01;

enum {
    PAIR_M1 = 0x01,
    PAIR_M3 = 0x03,
    PAIR_M5 = 0x05,
    PAIR_M7 = 0x07,
} Pair_State; 

enum {
    TYPE_METHOD = 0x00,
    TYPE_IDENTIFIER = 0x01,
    TYPE_STATE = 0x06,
    TYPE_SALT = 0x02,
    TYPE_SRP_B = 0x03,
    TYPE_SRP_M2 = 0x04
} Tags; 

const char *HTTP = {
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/pairing+tlv8\r\n"
    "Date: Tue, 04 Jul 2017 23:39:34 GMT\r\n"
    "Connection: keep-alive\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
};

static void server_close(struct tcp_pcb *pcb) {
   tcp_arg(pcb, NULL);
   tcp_sent(pcb, NULL);
   tcp_recv(pcb, NULL);
   tcp_close(pcb);

   printf("[TCP] Client disconnected...\n");
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

void send_response(struct tcp_pcb *pcb, uint8_t* payload, uint16_t d_length) {
    printf("[TCP] Writing payload with %d bytes.\n", d_length);
    uint8_t* data = (uint8_t*)malloc(MAX_WRITE_SIZE);
    uint16_t w_length = 0;
    
    memcpy(data, HTTP, WRITE_HEADER_SIZE);
    w_length += WRITE_HEADER_SIZE;

    char buffer[5];
    sprintf(buffer, "%05x", d_length);
    memcpy(data+w_length, buffer, 0x05);
    w_length += 0x05;

    memcpy(data+w_length, "\r\n", 0x02);
    w_length += 0x02;

    memcpy(data+w_length, payload, d_length);
    w_length += d_length;

    memcpy(data+w_length, "\r\n0\r\n\r\n", 0x07);
    w_length += 0x07;
    
    for(int i=0; i<w_length; i++) {
        printf("0x%.2x ", data[i]);
    }
    printf("\n");

    tcp_write(pcb, data, w_length, 0);
    printf("[TCP] Response sent!\n");
}

void handle_M1(struct tcp_pcb *pcb) {
    printf("(M1)!\n");
    srp_start();
    pairing_state = 0x02;

    uint8_t* data = (uint8_t*)malloc(500);
    uint16_t d_length = 0x00;

    tlv_encode_next(data, &d_length, TYPE_STATE, sizeof(pairing_state), &pairing_state);
    tlv_encode_next(data, &d_length, TYPE_SALT, 16, srp_getSalt());
    tlv_encode_next(data, &d_length, TYPE_SRP_B, 384, srp_getB());

    send_response(pcb, data, d_length);
    free(data);
}

void handle_M3(struct tcp_pcb *pcb) {
    printf("(M3)!\n");
    pairing_state = 0x04;

    uint8_t* data = (uint8_t*)malloc(100);
    uint16_t d_length = 0x00;

    tlv_encode_next(data, &d_length, TYPE_STATE, sizeof(pairing_state), &pairing_state);
    tlv_encode_next(data, &d_length, TYPE_SRP_M2, 64, srp_getM2());

    //send_response(pcb, data, d_length);
    free(data);
}

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
        uint8_t* payload = (uint8_t*)malloc(payload_len);
        memcpy(payload, &req[header_len], payload_len);

#if DEBUG_FLAG
        printf("[DEBUG] Header have %d bytes.\n", header_len);
        printf("[DEBUG] Payload have %d bytes.\n", payload_len);
        for(int i=0; i<payload_len; i++) {
            printf("0x%.2x ", payload[i]);
        }
        printf("\n");
#endif

        handle_request(payload, payload_len, pcb);
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
    xTaskCreate(&httpd_task, "HTTP Daemon", 128, NULL, 2, NULL);
}
