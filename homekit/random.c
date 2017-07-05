/*
 * random.c
 *
 *  Created on: Jun 10, 2015
 *      Author: tim
 */

#include <espressif/esp_common.h>

#include "random.h"

void random_create(uint8_t* p_result, uint8_t length) {
  for(int i=0; i<length; i++) {
    p_result[i] += rand() % 256;
  }
}

void randombytes(uint8_t* p_result, uint64_t length) {
  random_create(p_result, (uint8_t)length);
}
