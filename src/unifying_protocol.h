
#ifndef UNIFYING_PROTOCOL_H
#define UNIFYING_PROTOCOL_H

#include <stdint.h>
#include <string.h>

#include "unifying_data.h"
#include "unifying_error.h"

#ifdef __cplusplus
extern "C" {
#endif

enum unifying_error unifying_pair_step_1(struct unifying_state* state,
                                                    uint8_t id,
                                                    uint16_t product_id,
                                                    uint16_t device_type);

enum unifying_error unifying_pair_step_2(struct unifying_state* state,
                                                    uint32_t crypto,
                                                    uint32_t serial,
                                                    uint16_t capabilities);

enum unifying_error unifying_pair_step_3(struct unifying_state* state,
                                                    const char* name,
                                                    uint8_t name_length);

enum unifying_error unifying_pair_complete(struct unifying_state* state);

enum unifying_error unifying_set_timeout(struct unifying_state* state, uint16_t timeout);

enum unifying_error unifying_keep_alive(struct unifying_state* state, uint16_t timeout);

enum unifying_error unifying_hidpp_1_0(struct unifying_state* state);

enum unifying_error unifying_encrypted_keystroke(struct unifying_state* state,
                                                 const uint8_t keys[UNIFYING_KEYS_LEN],
                                                 uint8_t modifiers);

#ifdef __cplusplus
}
#endif

#endif
