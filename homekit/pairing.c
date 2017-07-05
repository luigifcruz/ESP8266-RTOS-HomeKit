#include <espressif/esp_common.h>

#include "crypto/tlv.h"
#include "crypto/crypto.h"
#include "tcp.h"
#include "enum.h"
#include "pairing.h"

void handle_M1(struct tcp_pcb *pcb) {
    printf("(M1)!\n");
    srp_start();
    uint8_t pairing_state = 0x02;

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
    uint8_t pairing_state = 0x04;

    uint8_t* data = (uint8_t*)malloc(100);
    uint16_t d_length = 0x00;

    tlv_encode_next(data, &d_length, TYPE_STATE, sizeof(pairing_state), &pairing_state);
    tlv_encode_next(data, &d_length, TYPE_SRP_M2, 64, srp_getM2());

    //send_response(pcb, data, d_length);
    free(data);
}