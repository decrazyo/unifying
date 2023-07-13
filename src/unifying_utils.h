
#ifndef UNIFYING_UTILS_H
#define UNIFYING_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "unifying_const.h"

#ifdef __cplusplus
extern "C" {
#endif

void unifying_uint16_pack(uint8_t packed[2], uint16_t number);
void unifying_uint16_unpack(uint16_t* number, const uint8_t packed[2]);

void unifying_uint32_pack(uint8_t packed[4], uint32_t number);
void unifying_uint32_unpack(uint32_t* number, const uint8_t packed[4]);

int16_t unifying_int12_clamp(int16_t number);

uint8_t unifying_checksum(const uint8_t* buffer, uint8_t length);
uint8_t unifying_checksum_verify(const uint8_t* buffer, uint8_t length);

uint8_t unifying_xnor(uint8_t first, uint8_t second);

void unifying_deobfuscate_aes_key(uint8_t aes_key[UNIFYING_AES_BLOCK_LEN],
                                  const uint8_t proto_aes_key[UNIFYING_AES_BLOCK_LEN]);

uint8_t unifying_next_channel(uint8_t current_channel);

void unifying_copy_reverse(uint8_t *reverse, const uint8_t *forward, uint8_t length);

void unifying_print_buffer(const uint8_t *buffer, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
