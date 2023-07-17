
/*!
 * \file unifying.h
 * \brief High level functions for transmitting and receiving Unifying payloads.
 */

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

/*!
 * Dequeue a received payload and perform basic verification.
 * 
 * \note    This function is intended for internal use only.
 *          This function may be made static in the future.
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[out]      receive_entry   Pointer to an entry pointer. Used to return the dequeued payload.
 *                                  The pointer is only guaranteed to be valid
 *                                  if this function returns \ref UNIFYING_SUCCESS.
 * \param[in]       length          Expected length of the received payload.
 *                                  Supplying 0 as the payload length will skip the length check.
 * 
 * \return  \ref UNIFYING_BUFFER_EMPTY_ERROR if \ref unifying_state.receive_buffer "state.receive_buffer"
 *          is empty.
 * \return  \ref UNIFYING_CHECKSUM_ERROR if the received payload's computed checksum
 *          does not match its stated checksum.
 * \return  \ref UNIFYING_LENGTH_ERROR if the payload's length differs from \p length
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_response(struct unifying_state* state,
                                      struct unifying_receive_entry** receive_entry,
                                      uint8_t length);

/*!
 * Dequeue a response payload and queue a HID++ payload for transmission.
 * 
 * \todo:   Define possible return values.
 * 
 * \note    This function is intended for internal use only.
 *          This function may be made static in the future.
 * 
 * \param[in,out]   state   Unifying state information.
 * 
 */
enum unifying_error unifying_hidpp_1_0(struct unifying_state* state)

/*!
 * Transmit and receive Unifying payloads at regular intervals.
 * 
 * \todo:   Define possible return values.
 * 
 * Transmit a queued payload shortly before the current timeout has elapsed.
 * If an unhandled response payload is bufferd then a
 * \ref unifying_hidpp_1_0_short "HID++" payload will be queued for transmission.
 * If no payload is queued for transmission then a
 * \ref unifying_keep_alive_request "keep-alive" payload will be queued for transmission..
 * 
 * If a payload was received in response to the transmission then it will be queued for later handling.
 * 
 * \note This function is expected to be called regularly by the user of this library.
 * 
 * \param[in,out]   state   Unifying state information.
 * 
 */
enum unifying_error unifying_tick(struct unifying_state* state);


/*!
 * Repeatedly call unifying_tick() until a condition is met.
 * 
 * \note    If all \p exit_on_* parameters are false then this function will never return.
 * 
 * \param[in,out]   state               Unifying state information.
 * \param[in]       exit_on_error       Return if unifying_tick() returns an error.
 * \param[in]       exit_on_transmit    Return if \ref unifying_state.transmit_buffer "state.transmit_buffer"
 *                                      is empty, implying that all payloads have been transmitted.
 * \param[in]       exit_on_receive     Return if \ref unifying_state.receive_buffer "state.receive_buffer"
 *                                      is not empty, implying that a payload has been received.
 * 
 * \return  Any error returned by unifying_tick if \p exit_on_error is true.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_loop(struct unifying_state* state,
                                  bool exit_on_error,
                                  bool exit_on_transmit,
                                  bool exit_on_receive);

/*!
 * Pair with a Unifying receiver.
 * 
 * \todo:   Define possible return values.
 * 
 * \note    Upon successfully pairing, this function will populate
 *          \ref unifying_state.address "state.address" and
 *          \ref unifying_state.aes_key "state.aes_key".
 *          Those values are expected to be saved to non-volatile storage by the caller.
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[in]       id              Random value used for verifying the early stage of the pairing process.
 * \param[in]       product_id      Product ID of your device.
 *                                  This value becomes part of the device's AES encryption key.
 *                                  For added security, this should be a cryptographically secure random number.
 * \param[in]       device_type     Values indicating the device type.
 *                                  Valid values and their meaning are not yet documented.
 * \param[in]       crypto          A cryptographically secure random number, used for AES encryption key generation.
 * \param[in]       serial          Serial number of your device. The exact value does not matter.
 * \param[in]       capabilities    HID++ capabilities.
 *                                  Valid values and their meaning are not yet documented.
 * \param[in]       name            Name of your device.
 *                                  This name will appear in the Logitech Unifying desktop software.
 *                                  This value does not need to be NULL terminated.
 *                                  The name cannot be longer than \ref UNIFYING_MAX_NAME_LEN.
 * \param[in]       name_length     Length of the supplied name.
 *                                  The name length does not include a NULL terminator.
 *                                  The name cannot be longer than \ref UNIFYING_MAX_NAME_LEN.
 * 
 */
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
