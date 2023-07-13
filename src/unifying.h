
#ifndef UNIFYING_H
#define UNIFYING_H

#include <stdbool.h>
#include <stdint.h>

#include "unifying_error.h"
#include "unifying_data.h"
#include "unifying_utils.h"
#include "unifying_protocol.h"
#include "unifying_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

enum unifying_error unifying_response(struct unifying_state* state,
                                          struct unifying_receive_entry** receive_entry,
                                          uint8_t length);

enum unifying_error unifying_tick(struct unifying_state* state);


enum unifying_error unifying_loop(struct unifying_state* state,
                                  bool exit_on_error,
                                  bool exit_on_transmit,
                                  bool exit_on_receive);

enum unifying_error unifying_pair(struct unifying_state* state,
                                  uint8_t id,
                                  uint16_t product_id,
                                  uint16_t device_type,
                                  uint32_t crypto,
                                  uint32_t serial,
                                  uint16_t capabilities,
                                  const char* name,
                                  uint8_t name_length);

#ifdef __cplusplus
}
#endif

#endif
