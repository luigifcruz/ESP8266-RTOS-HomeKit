#ifndef HOMEKIT_PAIRING_H_
#define HOMEKIT_PAIRING_H_

#include <lwip/tcp.h>

#define DEBUG_FLAG 0

void handle_M1(struct tcp_pcb *pcb);
void handle_M3(struct tcp_pcb *pcb);
void pairing_handler(uint8_t state, uint8_t* payload, uint16_t p_length, struct tcp_pcb *pcb);

#endif /* HOMEKIT_PAIRING_H_ */