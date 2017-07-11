/*
 * tlv.c
 *
 *  Created on: Jun 19, 2015
 *      Author: tim
 */

#include <stdint.h>
#include <string.h>
#include <espressif/esp_common.h>

#include "tlv.h"

uint8_t tlv_decode_next(uint8_t** data, uint16_t* dlength, bool* halt, uint8_t* type, uint16_t* length, uint8_t** value) {
  uint8_t* iptr = *data;
  uint8_t ilen = iptr[1];

  if (*halt) {
    return 0;
  }
  if (*dlength < 2 && ilen + 2 > *dlength) {
    return 0;
  } else {
    *type = *iptr;
    *length = ilen;
    *value = iptr + 2;
    *data = iptr + 2 + ilen;
    *dlength -= 2 + ilen;
    while (ilen == 255 && *dlength >= 2 && **data == *type) {
      iptr = *data;
      ilen = iptr[1];
      memcpy(iptr, iptr + 2, *dlength - 2);
      *length += ilen;
      *data = iptr + ilen;
      *dlength -= 2 + ilen;
    }
    return 1;
  }
}

uint8_t tlv_encode_next(uint8_t* data, uint16_t* dlength, uint8_t type, uint16_t length, const uint8_t* value) {
  while (length) {
    data[*dlength] = type;
    *dlength += 1;
    uint8_t piece = length > 255 ? 255 : length;
    data[*dlength] = piece;
    *dlength += 1;
    memcpy(&data[*dlength], value, piece);
    *dlength += piece;
    length -= piece;
  }
  return 1;
}

