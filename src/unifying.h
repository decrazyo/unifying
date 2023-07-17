
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
#include "unifying_state.h"
#include "unifying_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * Transmit and receive Unifying payloads at regular intervals.
 * 
 * \todo    Define possible return values.
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
 * \todo    Define possible return values.
 * 
 * \note    Upon successfully pairing, this function will populate
 *          \ref unifying_state.address "state.address" and
 *          \ref unifying_state.aes_key "state.aes_key".
 *          Those values are expected to be saved to non-volatile storage by the caller.
 * 
 * \todo    Document valid values for \p device_type
 * 
 * \todo    Document valid values for \p capabilities
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[in]       id              Random value used for verifying the early stage of the pairing process.
 * \param[in]       product_id      Product ID of your device.
 *                                  This value becomes part of the device's AES encryption key.
 *                                  For added security, this should be a cryptographically secure random number.
 * \param[in]       device_type     Values indicating the device type.
 * \param[in]       crypto          A cryptographically secure random number, used for AES encryption key generation.
 * \param[in]       serial          Serial number of your device. The exact value does not matter.
 * \param[in]       capabilities    HID++ capabilities.
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

/*!
 * Queue a payload that sets the timeout for keep-alive packets.
 * 
 * This can be useful for conserving power when the user isn't actively using the device.
 * 
 * \param[in,out]   state       Unifying state information.
 * \param[in]       timeout     New packet timeout.
 * 
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_set_timeout(struct unifying_state* state, uint16_t timeout);

/*!
 * Immediately transmit an encrypted keystroke payload.
 * 
 * \note    Sending 2 or more keyboard scancodes at once requires sending an intermediate payload
 *          for each additional keyboard scancode.
 *          Otherwise the Unifying receiver will reject the payload.
 * \code{.c}
 * // e.g. Pressing 'a', 'b', and 'c' keys at the same time.
 * uint8_t keys[UNIFYING_KEYS_LEN] = {0, 0, 0, 0, 0, 0};
 * keys[5] = 0x04; // scancode for 'a'
 * // Press 'a'.
 * unifying_encrypted_keystroke(&state, &keys, 0);
 * keys[4] = 0x05; // scancode for 'b'
 * // Press 'a' and 'b'.
 * unifying_encrypted_keystroke(&state, &keys, 0);
 * keys[3] = 0x06; // scancode for 'c'
 * // Press 'a', 'b', and 'c'.
 * unifying_encrypted_keystroke(&state, &keys, 0);
 * \endcode
 * 
 * \param[in,out]   state       Unifying state information.
 * \param[in]       keys        Pointer to a buffer of \ref UNIFYING_KEYS_LEN keyboard scancodes
 * \param[in]       modifiers   Bitfield where each bit corresponds to a specific modifier key.
 * 
 * \todo    Define modifiers key bits.
 * 
 * \return  \ref UNIFYING_ENCRYPTION_ERROR if payload encryption fails.
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_TRANSMIT_ERROR if payload transmission fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the receive buffer is full and a response payload is available.
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_PAYLOAD_LENGTH_ERROR if the response payload's length differs from its expected length.
 *          This should never happen.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_encrypted_keystroke(struct unifying_state* state,
                                                 const uint8_t keys[UNIFYING_KEYS_LEN],
                                                 uint8_t modifiers);


#ifdef __cplusplus
}
#endif

#endif
