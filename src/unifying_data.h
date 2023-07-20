
/*!
 * \file unifying_data.h
 * \brief Structures representing Unifying payload data.
 * 
 * Unifying payloads are represented here as structs in an attempt to document payload structure in code.
 * Functions for initializing, packing, and unpacking these structs are also provided here.
 * There may be a small performance loss from converting between uint8_t arrays and structs.
 * That performance loss is deemed acceptable for the time being.
 * 
 * \todo    Document struct fields.
 * 
 * \todo    move checksum calculation from *_init functions to *_pack functions.
 */

#ifndef UNIFYING_DATA_H
#define UNIFYING_DATA_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "unifying_const.h"
#include "unifying_utils.h"

// NOTE: Consider replacing some of the uint8_t arrays with pointers.

/*!
 * Pairing request payload number 1.
 * 
 * This is the initial paring request that is sent to a Unifying receiver.
 */
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

/*!
 * Pairing response payload number 1.
 * 
 * This is the expected response to \ref unifying_pair_request_1.
 * 
 * \see unifying_pair_request_1
 * \see unifying_keep_alive_request
 */
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

/*!
 * Pairing request payload number 2.
 * 
 * This paring request is sent after receiving a \ref unifying_pair_response_1 payload.
 * 
 * \see unifying_pair_response_1
 */
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

/*!
 * Pairing response payload number 2.
 * 
 * This is the expected response to \ref unifying_pair_request_2.
 * 
 * \see unifying_pair_request_2
 * \see unifying_keep_alive_request
 */
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

/*!
 * Pairing request payload number 3.
 * 
 * This paring request is sent after receiving a \ref unifying_pair_response_2 payload.
 * 
 * * \see unifying_pair_response_2
 */
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

/*!
 * Pairing response payload number 3.
 * 
 * This is the expected response to \ref unifying_pair_request_3.
 * 
 * \see unifying_pair_request_2
 * \see unifying_keep_alive_request
 */
struct unifying_pair_response_3
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    uint8_t unknown_3_8[6]; // Observed to contain crypto data from request/response 2.
    uint8_t checksum;
};

/*!
 * Pairing complete request payload.
 * 
 * This paring request is sent after receiving a \ref unifying_pair_response_3 payload.
 * No response payload is expected after this.
 * 
 * \see unifying_pair_response_3
 */
struct unifying_pair_complete_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t step;
    uint8_t unknown_3; // Observed as 0x01.
    uint8_t unknown_4_8[5];
    uint8_t checksum;
};

/*!
 * Long wake-up request payload.
 * 
 * This is used to re-connect a device to a paired receiver
 * after being powered off or going to sleep.
 * \see unifying_long_wake_up_request
 */
struct unifying_long_wake_up_request
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

/*!
 * Short wake-up request payload.
 * 
 * This is used to re-connect a device to a paired receiver
 * after being powered off or going to sleep.
 * 
 * \see unifying_long_wake_up_request
 */
struct unifying_short_wake_up_request
{
    uint8_t index;
    uint8_t frame;
    uint8_t unknown_2; // Observed as 0x01.
    uint8_t unknown_3; // Observed as 0x4B.
    uint8_t unknown_4; // Observed as 0x01.
    uint8_t unknown_5_8[4];
    uint8_t checksum;
};

/*!
 * Set timeout request payload.
 * 
 * Used to inform the paired receiver how often to expect keep-alive payloads.
 * Some other payloads implicitly set the timeout to a default value.
 * 
 * \see unifying_keep_alive_request
 */
struct unifying_set_timeout_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t unknown_2;
    uint16_t timeout;
    uint8_t unknown_5_8[4];
    uint8_t checksum;
};

/*!
 * Keep-alive request payload.
 * 
 * Used to inform the paired receiver that this device is still active.
 * Various response payloads will sometimes be received after transmitting a keep-alive.
 * 
 * \see unifying_pair_response_1
 * \see unifying_pair_response_2
 * \see unifying_pair_response_3
 * \see unifying_hidpp_1_0_short
 * \see unifying_hidpp_1_0_long
 */
struct unifying_keep_alive_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint16_t timeout;
    uint8_t checksum;
};

/*!
 * Short HID++ 1.0 payload.
 * 
 * This is used for HID++ 1.0 requests and responses.
 * 
 * \see unifying_keep_alive_request
 * \see unifying_hidpp_1_0_long
 */
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

/*!
 * Long HID++ 1.0 payload.
 * 
 * This is used for HID++ 1.0 requests and responses.
 * 
 * \see unifying_keep_alive_request
 * \see unifying_hidpp_1_0_short
 */
struct unifying_hidpp_1_0_long
{
    uint8_t unknown_0;
    uint8_t report;
    uint8_t index;
    uint8_t sub_id;
    uint8_t params[UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN];
    uint8_t checksum;
};

/*!
 * Encrypted keystroke request payload.
 * 
 * Used to transmit keyboard scancodes to a Unifying receiver.
 */
struct unifying_encrypted_keystroke_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t ciphertext[UNIFYING_AES_DATA_LEN];
    uint32_t counter;
    uint8_t unknown_14_20[7];
    uint8_t checksum;
};

/*!
 * Multimedia keystroke request payload.
 */
struct unifying_multimeia_keystroke_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t keys[UNIFYING_MULTIMEDIA_KEYS_LEN];
    uint8_t unknown_6_8[3];
    uint8_t checksum;
};

/*!
 * Mouse request payload.
 * 
 * Used to transmit mouse movement, clicking, and scrolling to a Unifying receiver.
 */
struct unifying_mouse_request
{
    uint8_t unknown_0;
    uint8_t frame;
    uint8_t buttons;
    uint8_t unknown_3;
    int16_t move_y;
    int16_t move_x;
    int8_t wheel_y;
    int8_t wheel_x;
    uint8_t checksum;
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Initialize a \ref unifying_pair_request_1 structure.
 * 
 * \param[out]  unpacked        Pointer to a \ref unifying_pair_request_1 to initialize.
 * \param[in]   id              Random value used for verifying the early stage of the pairing process.
 * \param[in]   timeout         Default device timeout.
 * \param[in]   product_id      The product ID of this device.
 * \param[in]   device_type     Values indicating the device type.
 * 
 * \see unifying_pair_request_1
 * \see UNIFYING_DEFAULT_TIMEOUT_KEYBOARD
 * \see UNIFYING_DEFAULT_TIMEOUT_MOUSE
 */
void unifying_pair_request_1_init(struct unifying_pair_request_1* unpacked,
                                  uint8_t id,
                                  uint16_t timeout,
                                  uint16_t product_id,
                                  uint16_t device_type);

/*!
 * Pack a \ref unifying_pair_request_1 into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_REQUEST_1_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_pair_request_1 to pack.
 */
void unifying_pair_request_1_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_1_LEN],
                                  const struct unifying_pair_request_1* unpacked);

/*!
 * Unpack a byte array into a \ref unifying_pair_response_1.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_pair_response_1 to unpack into.
 * \param[in]   packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_RESPONSE_1_LEN bytes long.
 */
void unifying_pair_response_1_unpack(struct unifying_pair_response_1* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_1_LEN]);

/*!
 * Initialize a \ref unifying_pair_request_2 structure.
 * 
 * \param[out]  unpacked        Pointer to a \ref unifying_pair_request_2 to initialize.
 * \param[in]   crypto          Cryptographically secure random number.
 * \param[in]   serial          Serial number of this device.
 * \param[in]   capabilities    HID++ capabilities.
 * 
 * \see unifying_pair_request_2
 */
void unifying_pair_request_2_init(struct unifying_pair_request_2* unpacked,
                                  uint32_t crypto,
                                  uint32_t serial,
                                  uint16_t capabilities);

/*!
 * Pack a \ref unifying_pair_request_2 into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_RESPONSE_2_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_pair_request_2 to pack.
 */
void unifying_pair_request_2_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_2_LEN],
                                  const struct unifying_pair_request_2* unpacked);

/*!
 * Unpack a byte array into a \ref unifying_pair_response_2.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_pair_response_2 to unpack into.
 * \param[in]   packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_RESPONSE_2_LEN bytes long.
 */
void unifying_pair_response_2_unpack(struct unifying_pair_response_2* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_2_LEN]);

/*!
 * Initialize a \ref unifying_pair_request_3 structure.
 * 
 * \param[out]  unpacked        Pointer to a \ref unifying_pair_request_3 to initialize.
 * \param[in]   name            Name of your device.
 * \param[in]   name_length     Length of \p name.
 * 
 * \see unifying_pair_request_3
 */
void unifying_pair_request_3_init(struct unifying_pair_request_3* unpacked, const char* name, uint8_t name_length);

/*!
 * Pack a \ref unifying_pair_request_3 into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_REQUEST_3_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_pair_request_3 to pack.
 */
void unifying_pair_request_3_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_3_LEN],
                                  const struct unifying_pair_request_3* unpacked);

/*!
 * Unpack a byte array into a \ref unifying_pair_response_3.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_pair_response_3 to unpack into.
 * \param[in]   packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_RESPONSE_3_LEN bytes long.
 */
void unifying_pair_response_3_unpack(struct unifying_pair_response_3* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_3_LEN]);

/*!
 * Initialize a \ref unifying_pair_complete_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_pair_complete_request to initialize.
 * 
 * \see unifying_pair_complete_request
 */
void unifying_pair_complete_request_init(struct unifying_pair_complete_request* unpacked);

/*!
 * Pack a \ref unifying_pair_complete_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_PAIR_COMPLETE_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_pair_complete_request to pack.
 */
void unifying_pair_complete_request_pack(uint8_t packed[UNIFYING_PAIR_COMPLETE_REQUEST_LEN],
                                         const struct unifying_pair_complete_request* unpacked);

/*!
 * Initialize a \ref unifying_long_wake_up_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_long_wake_up_request to initialize.
 * \param[in]   index       Least significant byte of the RF address for this device.
 * 
 * \see unifying_long_wake_up_request
 */
void unifying_long_wake_up_request_init(struct unifying_long_wake_up_request* unpacked, uint8_t index);

/*!
 * Pack a \ref unifying_long_wake_up_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_LONG_WAKE_UP_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_long_wake_up_request to pack.
 */
void unifying_long_wake_up_request_pack(uint8_t packed[UNIFYING_LONG_WAKE_UP_REQUEST_LEN],
                                     const struct unifying_long_wake_up_request* unpacked);

/*!
 * Initialize a \ref unifying_short_wake_up_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_short_wake_up_request to initialize.
 * \param[in]   index       Least significant byte of the RF address for this device.
 * 
 * \see unifying_short_wake_up_request
 */
void unifying_short_wake_up_request_init(struct unifying_short_wake_up_request* unpacked, uint8_t index);

/*!
 * Pack a \ref unifying_short_wake_up_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_SHORT_WAKE_UP_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_short_wake_up_request to pack.
 */
void unifying_short_wake_up_request_pack(uint8_t packed[UNIFYING_SHORT_WAKE_UP_REQUEST_LEN],
                                     const struct unifying_short_wake_up_request* unpacked);

/*!
 * Initialize a \ref unifying_set_timeout_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_set_timeout_request to initialize.
 * \param[in]   timeout     Timeout for keep-alive packets.
 * 
 * \see unifying_set_timeout_request
 */
void unifying_set_timeout_request_init(struct unifying_set_timeout_request* unpacked, uint16_t timeout);

/*!
 * Pack a \ref unifying_set_timeout_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_SET_TIMEOUT_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_set_timeout_request to pack.
 */
void unifying_set_timeout_request_pack(uint8_t packed[UNIFYING_SET_TIMEOUT_REQUEST_LEN],
                                     const struct unifying_set_timeout_request* unpacked);

/*!
 * Initialize a \ref unifying_keep_alive_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_keep_alive_request to initialize.
 * \param[in]   timeout     Timeout for keep-alive packets.
 * 
 * \see unifying_keep_alive_request
 */
void unifying_keep_alive_request_init(struct unifying_keep_alive_request* unpacked, uint16_t timeout);

/*!
 * Pack a \ref unifying_keep_alive_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_KEEP_ALIVE_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_keep_alive_request to pack.
 */
void unifying_keep_alive_request_pack(uint8_t packed[UNIFYING_KEEP_ALIVE_REQUEST_LEN],
                                     const struct unifying_keep_alive_request* unpacked);

/*!
 * Initialize a \ref unifying_hidpp_1_0_short structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_hidpp_1_0_short to initialize.
 * \param[in]   index       Least significant byte of the RF address for this device.
 *                          Or 0xFF if the payload originates from a receiver.
 * \param[in]   sub_id      HID++ 1.0 SubID.
 * \param[in]   params      Array of at least \ref UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN bytes
 *                          containing HID++ parameters.
 * 
 * \see unifying_hidpp_1_0_short
 */
void unifying_hidpp_1_0_short_init(struct unifying_hidpp_1_0_short* unpacked,
                                  uint8_t index,
                                  uint8_t sub_id,
                                  uint8_t params[UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN]);

/*!
 * Pack a \ref unifying_hidpp_1_0_short into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_HIDPP_1_0_SHORT_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_hidpp_1_0_short to pack.
 */
void unifying_hidpp_1_0_short_pack(uint8_t packed[UNIFYING_HIDPP_1_0_SHORT_LEN],
                                  const struct unifying_hidpp_1_0_short* unpacked);

/*!
 * Unpack a byte array into a \ref unifying_hidpp_1_0_short.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_hidpp_1_0_short to unpack into.
 * \param[in]   packed      Pointer to byte array that is at least \ref UNIFYING_HIDPP_1_0_SHORT_LEN bytes long.
 */
void unifying_hidpp_1_0_short_unpack(struct unifying_hidpp_1_0_short* unpacked,
                                     const uint8_t packed[UNIFYING_HIDPP_1_0_SHORT_LEN]);

/*!
 * Initialize a \ref unifying_hidpp_1_0_long structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_hidpp_1_0_long to initialize.
 * \param[in]   index       Least significant byte of the RF address for this device.
 *                          Or 0xFF if the payload originates from a receiver.
 * \param[in]   sub_id      HID++ 1.0 SubID.
 * \param[in]   params      Array of at least \ref UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN bytes
 *                          containing HID++ parameters.
 * 
 * \see unifying_hidpp_1_0_long
 */
void unifying_hidpp_1_0_long_init(struct unifying_hidpp_1_0_long* unpacked,
                                 uint8_t index,
                                 uint8_t sub_id,
                                 uint8_t params[UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN]);

/*!
 * Pack a \ref unifying_hidpp_1_0_long into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_HIDPP_1_0_LONG_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_hidpp_1_0_long to pack.
 */
void unifying_hidpp_1_0_long_pack(uint8_t packed[UNIFYING_HIDPP_1_0_LONG_LEN],
                                 const struct unifying_hidpp_1_0_long* unpacked);

/*!
 * Unpack a byte array into a \ref unifying_hidpp_1_0_long.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_hidpp_1_0_long to unpack into.
 * \param[in]   packed      Pointer to byte array that is at least \ref UNIFYING_HIDPP_1_0_LONG_LEN bytes long.
 */
void unifying_hidpp_1_0_long_unpack(struct unifying_hidpp_1_0_long* unpacked,
                                     const uint8_t packed[UNIFYING_HIDPP_1_0_LONG_LEN]);

/*!
 * Initialize a \ref unifying_encrypted_keystroke_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_encrypted_keystroke_request to initialize.
 * \param[in]   ciphertext  Array of at least \ref UNIFYING_AES_DATA_LEN bytes of encrypted keystroke data.
 * \param[in]   counter     AES counter.
 * 
 * \see unifying_encrypted_keystroke_request
 */
void unifying_encrypted_keystroke_request_init(struct unifying_encrypted_keystroke_request* unpacked,
                                               uint8_t ciphertext[UNIFYING_AES_DATA_LEN],
                                               uint32_t counter);

/*!
 * Pack a \ref unifying_encrypted_keystroke_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least
 *                          \ref UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_encrypted_keystroke_request to pack.
 */
void unifying_encrypted_keystroke_request_pack(uint8_t packed[UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN],
                                               const struct unifying_encrypted_keystroke_request* unpacked);

/*!
 * Initialize a \ref unifying_multimeia_keystroke_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_multimeia_keystroke_request to initialize.
 * \param[in]   keys        Array of at least \ref UNIFYING_AES_DATA_LEN bytes of multimedia keystroke data.
 * 
 * \see unifying_multimeia_keystroke_request
 */
void unifying_multimeia_keystroke_request_init(struct unifying_multimeia_keystroke_request* unpacked,
                                               uint8_t keys[UNIFYING_MULTIMEDIA_KEYS_LEN]);

/*!
 * Pack a \ref unifying_multimeia_keystroke_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least
 *                          \ref UNIFYING_MULTIMEDIA_KEYSTROKE_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_multimeia_keystroke_request to pack.
 */
void unifying_multimeia_keystroke_request_pack(uint8_t packed[UNIFYING_MULTIMEDIA_KEYSTROKE_REQUEST_LEN],
                                               const struct unifying_multimeia_keystroke_request* unpacked);

/*!
 * Initialize a \ref unifying_mouse_request structure.
 * 
 * \param[out]  unpacked    Pointer to a \ref unifying_mouse_request to initialize.
 * \param[in]   buttons     Bitfield where each bit corresponds to a mouse button.
 * \param[in]   move_y      Y axis mouse movement.
 * \param[in]   move_x      X axis mouse movement.
 * \param[in]   wheel_y     Y axis scroll wheel movement.
 * \param[in]   wheel_x     X axis scroll wheel movement.
 * 
 * \note    X and Y movement is packed as a pair of big-endian signed 12-bit integers.
 *          The X and Y movement data is expected to have already been clamped to a signed 12-bit range
 *          with unifying_clamp_int12() prior to calling this function.
 * 
 * \see unifying_mouse_request
 */
void unifying_mouse_request_init(struct unifying_mouse_request* unpacked,
                                 uint8_t buttons,
                                 int16_t move_y,
                                 int16_t move_x,
                                 int8_t wheel_y,
                                 int8_t wheel_x);

/*!
 * Pack a \ref unifying_mouse_request into a byte array.
 * 
 * \param[out]  packed      Pointer to byte array that is at least \ref UNIFYING_MOUSE_REQUEST_LEN bytes long.
 * \param[in]   unpacked    Pointer to a \ref unifying_mouse_request to pack.
 */
void unifying_mouse_request_pack(uint8_t packed[UNIFYING_MOUSE_REQUEST_LEN],
                                      const struct unifying_mouse_request* unpacked);


#ifdef __cplusplus
}
#endif

#endif
