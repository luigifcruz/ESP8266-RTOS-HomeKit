#include <espressif/esp_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lwip/tcp.h>

#include "tcp.h"
#include "crypto/crypto.h"

session_keys_t session_keys;

static struct {
  uint8_t encrypting;
  uint8_t havekeys;
} session_state;

const char *HTTP_HEADER = {
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/pairing+tlv8\r\n"
    "Date: Tue, 04 Jul 2017 23:39:34 GMT\r\n"
    "Connection: keep-alive\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
};

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

void session_readData(uint8_t* plaintext, uint16_t length, uint8_t* ciphertext, uint16_t* clength) {
  if (session_state.encrypting) {
    session_genkeys();
    crypto_transportEncrypt(session_keys.transport.read, session_keys.transport.read_nonce, plaintext, length, ciphertext, clength);
  } else {
    memcpy(ciphertext, plaintext, length);
    *clength = length;
  }
}

uint8_t session_writeData(uint8_t* ciphertext, uint16_t length, uint8_t* plaintext, uint16_t* plength) {
  if (session_state.encrypting) {
    session_genkeys();
    return crypto_transportDecrypt(session_keys.transport.write, session_keys.transport.write_nonce, ciphertext, length, plaintext, plength);
  } else {
    memcpy(ciphertext, plaintext, length);
    *plength = length;
    return 1;
  }
}

void server_close(struct tcp_pcb *pcb) {
   tcp_arg(pcb, NULL);
   tcp_sent(pcb, NULL);
   tcp_recv(pcb, NULL);
   tcp_close(pcb);

   printf("[TCP] Client disconnected...\n");
}

void send_response(struct tcp_pcb *pcb, uint8_t* payload, uint16_t d_length) {
    printf("[TCP] Writing payload with %d bytes.\n", d_length);
    uint8_t* data = (uint8_t*)malloc(MAX_WRITE_SIZE);
    uint16_t w_length = 0;
    
    // Write Request Header.
    memcpy(data, HTTP_HEADER, WRITE_HEADER_SIZE);
    w_length += WRITE_HEADER_SIZE;

    // Write Chunk Length.
    char buffer[5];
    sprintf(buffer, "%05x", d_length);
    memcpy(data+w_length, buffer, 0x05);
    w_length += 0x05;

    memcpy(data+w_length, "\r\n", 0x02);
    w_length += 0x02;

    // Write TLV Data. 
    memcpy(data+w_length, payload, d_length);
    w_length += d_length;

    // Write last chunk. 
    memcpy(data+w_length, "\r\n0\r\n\r\n", 0x07);
    w_length += 0x07;

    tcp_write(pcb, data, w_length, 0);
    printf("[TCP] Response sent!\n");
}

err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
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
#endif

        handle_request(payload, payload_len, pcb);
        pbuf_free(p);
    } else {
        pbuf_free(p);
        server_close(pcb);
    }

    return ERR_OK;
}

void session_genkeys(void) {
  if (!session_state.havekeys) {
    session_state.havekeys = 1;
    uint8_t key[64];
    crypto_hkdf(key, "Control-Salt", 12, "Control-Read-Encryption-Key\001", 28, session_keys.shared, sizeof(session_keys.shared));
    memcpy(session_keys.transport.read, key, 32);
    crypto_hkdf(key, "Control-Salt", 12, "Control-Write-Encryption-Key\001", 29, session_keys.shared, sizeof(session_keys.shared));
    memcpy(session_keys.transport.write, key, 32);
    memset(session_keys.transport.read_nonce, 0, sizeof(session_keys.transport.read_nonce));
    memset(session_keys.transport.write_nonce, 0, sizeof(session_keys.transport.write_nonce));
  }
}

err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err) {
    printf("[TCP] New client connected!\n");

    /*random_create(session_keys.verify.secret, sizeof(session_keys.verify.secret));
    crypto_scalarmult_base(session_keys.verify.public, session_keys.verify.secret);
    session_keys.verify.secret[0] &= 248;
    session_keys.verify.secret[31] = (session_keys.verify.secret[31] & 127) | 64;
    session_state.encrypting = 0;
    session_state.havekeys = 0;*/

    tcp_recv(pcb, server_recv);
    tcp_accepted(pcb);
    return ERR_OK;
}

void tcp_task_init(void *pvParameters) {
    struct tcp_pcb *pcb;

    // Start TCP server and enable Keepalive.
    pcb = tcp_new();
    pcb->so_options |= SOF_KEEPALIVE;
    tcp_bind(pcb, IP_ADDR_ANY, IP_ADDRESS);

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, server_accept);
    lwip_init();

    for (;;);
}