
/*!
 * \file unifying_data.h
 * \brief Structures representing Unifying payload data.
 * 
 * Unifying payloads are represented here as structs in an attempt to document payload structure in code.
 * Functions for initializing, packing, and unpacking these structs are also provided here.
 * There may be a small performance loss from converting between uint8_t arrays and structs.
 * That performance loss is deemed acceptable for the time being.
 */

#ifndef UNIFYING_DATA_H
#define UNIFYING_DATA_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "unifying_const.h"
#include "unifying_buffer.h"
#include "unifying_utils.h"

// NOTE: Consider replacing some of the uint8_t arrays with pointers.

struct unifying_pair_request_1
{
    uint8_t id;
    uint8_t frame;
    uint8_t step;
    uint8_t unknown_3_7[5]; // Previously paired RF address or random data.
    uint8_t timeout; // Observed as 0x08.
    uint16_t product_id;
    uint8_t protocol;
    uint8_t unknown_12;
    uint16_t device_type;
    uint8_t unknown_15_19[5];
    uint8_t unknown_20; // Observed as non-zero.
    uint8_t checksum;
};

struct unifying_pair_response_1
{
    uint8_t id;
    uint8_t frame;
    uint8_t step;
    uint8_t address[UNIFYING_ADDRESS_LEN];
    uint8_t unknown_8; // Observed as 0x08.
    uint16_t product_id;
    uint8_t unknown_11_12[2];
    uint16_t device_type;
    uint8_t unknown_15_20[6];
    uint8_t checksum;
};

struct unifying_pair_request_2
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    uint32_t crypto;
    uint32_t serial;
    uint16_t capabilities;
    uint8_t unknown_13_20[8];
    uint8_t checksum;
};

struct unifying_pair_response_2
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    uint32_t crypto;
    uint32_t serial;
    uint16_t capabilities;
    uint8_t unknown_13_20[8];
    uint8_t checksum;
};

struct unifying_pair_request_3
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    // TODO: Test if this indicate the number of name packets.
    //       Perhaps this would allow names to be longer than 16 bytes.
    uint8_t unknown_3; // Observed as 0x01.
    uint8_t name_length; // Name length does not include a null terminator.
    char name[UNIFYING_MAX_NAME_LEN]; // Name does not need to include a null terminator.
    uint8_t checksum;
};

struct unifying_pair_response_3
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    uint8_t unknown_3_8[6]; // Observed to contain crypto data from request/response 2.
    uint8_t checksum;
};

struct unifying_pair_complete_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    uint8_t unknown_3; // Observed as 0x01.
    uint8_t unknown_4_8[5];
    uint8_t checksum;
};

struct unifying_proto_aes_key
{
    uint8_t base_address[UNIFYING_ADDRESS_LEN - 1];
    uint16_t device_product_id;
    uint16_t receiver_product_id;
    uint32_t device_crypto;
    uint32_t receiver_crypto;
};

struct unifying_wake_up_request_1
{
    uint8_t index;
    uint8_t frame;
    uint8_t index_2;
    uint8_t unknown_3;
    uint8_t unknown_4; // Observed as 0x00.
    uint8_t unknown_5_7[3]; // All observed as 0x01.
    uint8_t unknown_8_20[13];
    uint8_t checksum;
};

struct unifying_wake_up_request_2
{
    uint8_t index;
    uint8_t frame;
    uint8_t unknown_2; // Observed as 0x01.
    uint8_t unknown_3; // Observed as 0x4B.
    uint8_t unknown_4; // Observed as 0x01.
    uint8_t unknown_5_8[4];
    uint8_t checksum;
};

struct unifying_set_timeout_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t unknown_2;
    uint16_t timeout;
    uint8_t unknown_5_8[4];
    uint8_t checksum;
};

struct unifying_keep_alive_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint16_t timeout;
    uint8_t checksum;
};

struct unifying_hidpp_1_0_short
{
    uint8_t unknown_0;
    uint8_t report;
    uint8_t index;
    uint8_t sub_id;
    uint8_t params[UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN];
    uint8_t unknown_8;
    uint8_t checksum;
};

struct unifying_hidpp_1_0_long
{
    uint8_t unknown_0;
    uint8_t report;
    uint8_t index;
    uint8_t sub_id;
    uint8_t params[UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN];
    uint8_t checksum;
};

struct unifying_encrypted_keystroke_plaintext
{
    uint8_t modifiers;
    uint8_t keys[UNIFYING_KEYS_LEN];
    uint8_t flag;
};

struct unifying_encrypted_keystroke_iv
{
    uint8_t prefix[UNIFYING_AES_NONCE_PREFIX_LEN];
    uint32_t counter;
    uint8_t suffix[UNIFYING_AES_NONCE_SUFFIX_LEN];
};

struct unifying_encrypted_keystroke_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t ciphertext[UNIFYING_AES_DATA_LEN];
    uint32_t counter;
    uint8_t unknown_14_20[7];
    uint8_t checksum;
};

struct unifying_mouse_move_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t buttons;
    uint8_t unknown_3;
    int16_t move_x;
    int16_t move_y;
    int8_t wheel_x;
    int8_t wheel_y;
    uint8_t checksum;
};

#ifdef __cplusplus
extern "C" {
#endif

void unifying_pair_request_1_init(struct unifying_pair_request_1* unpacked,
                                  uint8_t id,
                                  uint16_t timeout,
                                  uint16_t product_id,
                                  uint16_t device_type);
void unifying_pair_request_1_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_1_LEN],
                                  const struct unifying_pair_request_1* unpacked);
void unifying_pair_response_1_unpack(struct unifying_pair_response_1* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_1_LEN]);

void unifying_pair_request_2_init(struct unifying_pair_request_2* unpacked,
                                  uint32_t crypto,
                                  uint32_t serial,
                                  uint16_t capabilities);
void unifying_pair_request_2_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_2_LEN],
                                  const struct unifying_pair_request_2* unpacked);
void unifying_pair_response_2_unpack(struct unifying_pair_response_2* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_2_LEN]);

void unifying_pair_request_3_init(struct unifying_pair_request_3* unpacked, const char* name, uint8_t name_length);
void unifying_pair_request_3_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_3_LEN],
                                  const struct unifying_pair_request_3* unpacked);
void unifying_pair_response_3_unpack(struct unifying_pair_response_3* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_3_LEN]);

void unifying_pair_complete_request_init(struct unifying_pair_complete_request* unpacked);
void unifying_pair_complete_request_pack(uint8_t packed[UNIFYING_PAIR_COMPLETE_REQUEST_LEN],
                                         const struct unifying_pair_complete_request* unpacked);

void unifying_proto_aes_key_init(struct unifying_proto_aes_key* unpacked,
                                     uint8_t base_address[UNIFYING_ADDRESS_LEN - 1],
                                     uint16_t device_product_id,
                                     uint16_t receiver_product_id,
                                     uint32_t device_crypto,
                                     uint32_t receiver_crypto);
void unifying_proto_aes_key_pack(uint8_t packed[UNIFYING_AES_BLOCK_LEN],
                                 const struct unifying_proto_aes_key* unpacked);

void unifying_wake_up_request_1_init(struct unifying_wake_up_request_1* unpacked, uint8_t index);
void unifying_wake_up_request_1_pack(uint8_t packed[UNIFYING_WAKE_UP_REQUEST_1_LEN],
                                     const struct unifying_wake_up_request_1* unpacked);

void unifying_wake_up_request_2_init(struct unifying_wake_up_request_2* unpacked, uint8_t index);
void unifying_wake_up_request_2_pack(uint8_t packed[UNIFYING_WAKE_UP_REQUEST_2_LEN],
                                     const struct unifying_wake_up_request_2* unpacked);

void unifying_set_timeout_request_init(struct unifying_set_timeout_request* unpacked, uint16_t timeout);
void unifying_set_timeout_request_pack(uint8_t packed[UNIFYING_SET_TIMEOUT_REQUEST_LEN],
                                     const struct unifying_set_timeout_request* unpacked);

void unifying_keep_alive_request_init(struct unifying_keep_alive_request* unpacked, uint16_t timeout);
void unifying_keep_alive_request_pack(uint8_t packed[UNIFYING_KEEP_ALIVE_REQUEST_LEN],
                                     const struct unifying_keep_alive_request* unpacked);

void unifying_hidpp_1_0_short_init(struct unifying_hidpp_1_0_short* unpacked,
                                  uint8_t index,
                                  uint8_t sub_id,
                                  uint8_t params[UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN]);
void unifying_hidpp_1_0_short_pack(uint8_t packed[UNIFYING_HIDPP_1_0_SHORT_LEN],
                                  const struct unifying_hidpp_1_0_short* unpacked);
void unifying_hidpp_1_0_short_unpack(struct unifying_hidpp_1_0_short* unpacked,
                                     const uint8_t packed[UNIFYING_HIDPP_1_0_SHORT_LEN]);

void unifying_hidpp_1_0_long_init(struct unifying_hidpp_1_0_long* unpacked,
                                 uint8_t index,
                                 uint8_t sub_id,
                                 uint8_t params[UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN]);
void unifying_hidpp_1_0_long_pack(uint8_t packed[UNIFYING_HIDPP_1_0_LONG_LEN],
                                 const struct unifying_hidpp_1_0_long* unpacked);
void unifying_hidpp_1_0_long_unpack(struct unifying_hidpp_1_0_long* unpacked,
                                     const uint8_t packed[UNIFYING_HIDPP_1_0_LONG_LEN]);

void unifying_encrypted_keystroke_plaintext_init(struct unifying_encrypted_keystroke_plaintext* unpacked,
                                                 uint8_t modifiers,
                                                 const uint8_t keys[UNIFYING_KEYS_LEN]);
void unifying_encrypted_keystroke_plaintext_pack(uint8_t packed[UNIFYING_AES_DATA_LEN],
                                                 const struct unifying_encrypted_keystroke_plaintext* unpacked);

void unifying_encrypted_keystroke_iv_init(struct unifying_encrypted_keystroke_iv* unpacked,
                                          uint32_t counter);
void unifying_encrypted_keystroke_iv_pack(uint8_t packed[UNIFYING_AES_BLOCK_LEN],
                                          const struct unifying_encrypted_keystroke_iv* unpacked);

void unifying_encrypted_keystroke_request_init(struct unifying_encrypted_keystroke_request* unpacked,
                                               uint8_t ciphertext[UNIFYING_AES_DATA_LEN],
                                               uint32_t counter);
void unifying_encrypted_keystroke_request_pack(uint8_t packed[UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN],
                                               const struct unifying_encrypted_keystroke_request* unpacked);

// TODO: Declare other keystroke types here.

void unifying_mouse_move_request_init(struct unifying_mouse_move_request* unpacked,
                                      uint8_t buttons,
                                      int16_t move_x,
                                      int16_t move_y,
                                      int8_t wheel_x,
                                      int8_t wheel_y);
void unifying_mouse_move_request_pack(uint8_t packed[UNIFYING_MOUSE_MOVE_REQUEST_LEN],
                                      const struct unifying_mouse_move_request* unpacked);


#ifdef __cplusplus
}
#endif

#endif
