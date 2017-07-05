#ifndef HOMEKIT_TCP_H_
#define HOMEKIT_TCP_H_

#include <lwip/tcp.h>

#define IP_ADDRESS 51062
#define DEBUG_FLAG 1
#define MAX_WRITE_SIZE 1024
#define WRITE_HEADER_SIZE 148

#include <lwip/tcp.h>

void server_close(struct tcp_pcb *pcb);
char* get_value(char* line);
uint16_t get_payload_len(char* req);
void send_response(struct tcp_pcb *pcb, uint8_t* payload, uint16_t d_length);
err_t server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
err_t server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
void handle_request(uint8_t* payload, uint16_t p_lenght, struct tcp_pcb *pcb);
void tcp_task_init(void *pvParameters);

#endif /* HOMEKIT_TCP_H_ */