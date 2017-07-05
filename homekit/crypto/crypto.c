/*
 * crypto.c
 *
 *  Created on: Jun 25, 2015
 *      Author: tim
 */

#include <stdint.h>
#include <string.h>

#include <espressif/esp_common.h>

#include "crypto.h"

#define CRYPTO_INSTANCE  2   // Change this to force key regeneration on next run

crypto_keys_t crypto_keys;
crypto_persistent_keys_t keys = {};

static const uint8_t zeros64[64];
static volatile uint8_t crypto_storing;
static uint8_t crypto_loadKeys(void);

void crypto_init(void)
{
  if (!crypto_loadKeys())
  {
    srp_init();

    crypto_sign_keypair(crypto_keys.sign.public, crypto_keys.sign.secret);

    // Store for reuse
    crypto_scheduleStoreKeys();
    crypto_storeKeys();
  }
}

static uint8_t crypto_loadKeys(void)
{
  if (keys.valid0 == 0x55 && keys.valid1 == 0xAA && keys.instance == CRYPTO_INSTANCE)
  {
    // Valid
    memcpy(srp.b, keys.srp_b, 32);
    memcpy(srp.salt, keys.srp_salt, 16);
    memcpy(srp.v, keys.srp_v, 384);
    memcpy(srp.B, keys.srp_B, 384);
    memcpy(crypto_keys.sign.secret, keys.sign_secret, 64);
    memcpy(crypto_keys.client.name, keys.clientname, 36);
    memcpy(crypto_keys.client.ltpk, keys.ltpk, 32);
    return 1;
  }
  else
  {
    return 0;
  }
}

void crypto_scheduleStoreKeys(void)
{
    crypto_storing = 1;
}

void crypto_storeKeys(void)
{
  uint32_t err_code;

  if (crypto_storing)
  {
    memcpy(keys.srp_b, srp.b, 32);
    memcpy(keys.srp_salt, srp.salt, 16);
    memcpy(keys.srp_v, srp.v, 384);
    memcpy(keys.srp_B, srp.B, 384);
    memcpy(keys.sign_secret, crypto_keys.sign.secret, 64);
    memcpy(keys.clientname, crypto_keys.client.name, 36);
    memcpy(keys.ltpk, crypto_keys.client.ltpk, 32);

    keys.instance = CRYPTO_INSTANCE;
    keys.valid0 = 0x55;
    keys.valid1 = 0xAA;
  }
}

uint8_t crypto_advertise(void)
{
  // If we have a client name, we don't advertise, otherwise we do.
  if (crypto_keys.client.name[0])
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

uint8_t crypto_verifyAndDecrypt(const uint8_t* key, uint8_t* nonce, uint8_t* encrypted, uint8_t length, uint8_t* output_buf, uint8_t* mac)
{
  uint8_t polykey[sizeof(zeros64)];
  crypto_stream_chacha20_xor(polykey, zeros64, sizeof(zeros64), nonce, key, 0);

  uint8_t padding = (16 - length % 16) % 16;
  uint8_t message[length + padding + 16];
  memcpy(message, encrypted, length);
  memset(message + length, 0, padding + 16);
  message[length + padding + 8] = (uint8_t)length;
  message[length + padding + 9] = (uint8_t)(length >> 8);

  if (crypto_onetimeauth_poly1305_verify(mac, message, sizeof(message), polykey) != 0)
  {
    // Fail
    return 0;
  }
  else
  {
    crypto_stream_chacha20_xor(output_buf, message, length, nonce, key, 1);
    return 1;
  }
}

void crypto_encryptAndSeal(const uint8_t* key, uint8_t* nonce, uint8_t* plain, uint16_t length, uint8_t* output_buf, uint8_t* output_mac)
{
  uint8_t polykey[sizeof(zeros64)];
  crypto_stream_chacha20_xor(polykey, zeros64, sizeof(zeros64), nonce, key, 0);

  uint8_t padding = (16 - length % 16) % 16;
  uint8_t message[length + padding + 16];

  crypto_stream_chacha20_xor(message, plain, length, nonce, key, 1);
  memset(message + length, 0, padding + 16);
  message[length + padding + 8] = (uint8_t)length;
  message[length + padding + 9] = (uint8_t)(length >> 8);

  crypto_onetimeauth_poly1305(output_mac, message, sizeof(message), polykey);

  memcpy(output_buf, message, length);
}

void crypto_sha512hmac(uint8_t* hash, uint8_t* salt, uint8_t salt_length, uint8_t* data, uint8_t data_length)
{
  uint8_t message1[128 + data_length];
  uint8_t message2[128 + 64];

  memset(message1, 0x36, 128);
  memset(message2, 0x5C, 128);
  for (unsigned i = salt_length; i--; )
  {
    message1[i] = 0x36 ^ salt[i];
    message2[i] = 0x5C ^ salt[i];
  }
  memcpy(message1 + 128, data, data_length);
  crypto_hash_sha512(message2 + 128, message1, sizeof(message1));
  crypto_hash_sha512(hash, message2, sizeof(message2));
}

void crypto_hkdf(uint8_t* target, uint8_t* salt, uint8_t salt_length, uint8_t* info, uint8_t info_length, uint8_t* ikm, uint8_t ikm_length)
{
  crypto_sha512hmac(target, salt, salt_length, ikm, ikm_length);
  crypto_sha512hmac(target, target, 64, info, info_length);
}

void crypto_transportEncrypt(uint8_t* key, uint8_t* nonce, uint8_t* plaintext, uint16_t plength, uint8_t* ciphertext, uint16_t* clength)
{
  crypto_encryptAndSeal(key, nonce, plaintext, plength, ciphertext, ciphertext + plength);
  for (unsigned i = 0; i < 8 && !++nonce[i]; i++)
    ;
  *clength = plength + 16;
}

uint8_t crypto_transportDecrypt(uint8_t* key, uint8_t* nonce, uint8_t* ciphertext, uint16_t clength, uint8_t* plaintext, uint16_t* plength)
{
  uint8_t r = crypto_verifyAndDecrypt(key, nonce, ciphertext, clength - 16, plaintext, ciphertext + clength - 16);
  for (unsigned i = 0; i < 8 && !++nonce[i]; i++)
    ;
  *plength = clength - 16;
  return r;
}
