#ifndef HOMEKIT_TCP_H_
#define HOMEKIT_TCP_H_

#include <lwip/tcp.h>

// Calculate the size of the cipher buffer based no the plaintext length (16 bytes of overhead)
#define SESSION_CIPHER_BUFFERLEN(PLAINLEN)  ((PLAINLEN) + 16)
#define SESSION_PLAIN_BUFFERLEN(CIPHERLEN)  ((CIPHERLEN) - 16)
#define IP_ADDRESS 51062
#define DEBUG_FLAG 0
#define MAX_WRITE_SIZE 1024
#define WRITE_HEADER_SIZE 148

typedef struct
{
  uint8_t     pve[64];

  // Shared secret established between client and accessory
  uint8_t     shared[32];

  // Keys and nonces used during encrypted comms
  struct
  {
    uint8_t   read[32];
    uint8_t   write[32];
    uint8_t   read_nonce[8];
    uint8_t   write_nonce[8];
  } transport;

  // DH keys
  struct
  {
    uint8_t   secret[32];
    uint8_t   public[32];
  } verify;

  // Client keys
  struct
  {
    uint8_t   public[32];
  } client;
} session_keys_t;

extern session_keys_t session_keys;

void server_close(struct tcp_pcb *pcb);
char* get_value(char* line);
void session_genkeys(void);
uint16_t get_payload_len(char* req);
void send_response(struct tcp_pcb *pcb, uint8_t* payload, uint16_t d_length);
err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
void handle_request(uint8_t* payload, uint16_t p_length, struct tcp_pcb *pcb);
void tcp_task_init(void *pvParameters);

#endif /* HOMEKIT_TCP_H_ */