#ifndef HOMEKIT_H_
#define HOMEKIT_H_

#include <lwip/tcp.h>

#define DEBUG_FLAG 1

void handle_request(uint8_t* payload, uint16_t p_lenght, struct tcp_pcb *pcb);
void homekit_init();

#endif /* HOMEKIT_H_ */