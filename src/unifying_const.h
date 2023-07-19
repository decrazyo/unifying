
/*!
 * \file unifying_const.h
 * \brief Constants used by the Unifying protocol
 * 
 * \todo    Define HID++ 1.0 errors as an enum
 * 
 * \todo    Add definitions for constants used in \ref unifying_data.c
 */

#ifndef UNIFYING_CONST_H
#define UNIFYING_CONST_H

#include <stdint.h>

#ifndef UNIFYING_TIMEOUT_COEFFICIENT
/*!
 * Determines what percent of the timeout should elapse before transmitting a keep-alive packet.
 * 
 * This can be re-defined by this library's user.
 * Defining this as a number greater than `1` will most like cause packets to timeout.
 */
#define UNIFYING_TIMEOUT_COEFFICIENT 0.875
#endif

/*!
 * Used to indicate that \ref unifying_transmit should not change the current timeout.
 */
#define UNIFYING_TIMEOUT_UNCHANGED 0

/*!
 * Size of an AES-128 block in bytes.
 */
#define UNIFYING_AES_BLOCK_LEN 16

/*!
 * Size of the AES nonce prefix in bytes.
 * \see unifying_aes_nonce_prefix
 */
#define UNIFYING_AES_NONCE_PREFIX_LEN 7

/*!
 * Size of the AES nonce suffix in bytes.
 * \see unifying_aes_nonce_suffix
 */
#define UNIFYING_AES_NONCE_SUFFIX_LEN 5

/*!
 * Size of encrypted data in bytes.
 */
#define UNIFYING_AES_DATA_LEN 8

/*!
 * Number of RF channels that the Unifying protocol uses in normal operation.
 * /see unifying_channels
 */

#define UNIFYING_CHANNELS_LEN 25
/*!
 * Number of RF channels that the Unifying protocol uses for pairing.
 * /see unifying_pairing_channels
 */

#define UNIFYING_PAIRING_CHANNELS_LEN 11

/*!
 * Size of the RF address in bytes.
 * /see unifying_pairing_address
 */
#define UNIFYING_ADDRESS_LEN 5

/*!
 * Maximum size of a Unifying payload in bytes.
 */
#define UNIFYING_MAX_PAYLOAD_LEN 22

/*!
 * Maximum name length for a Unifying device.
 * 
 * This length does not include a NULL terminator.
 */
#define UNIFYING_MAX_NAME_LEN 16

/*!
 * Number keyboard scancodes that can be transmitted in a single encrypted payload.
 */
#define UNIFYING_KEYS_LEN 6

/*!
 * Size of the first pairing request in bytes.
 */
#define UNIFYING_PAIR_REQUEST_1_LEN 22

/*!
 * Size of the first pairing response in bytes.
 */

#define UNIFYING_PAIR_RESPONSE_1_LEN 22

/*!
 * Size of the second pairing request in bytes.
 */
#define UNIFYING_PAIR_REQUEST_2_LEN 22

/*!
 * Size of the second pairing response in bytes.
 */
#define UNIFYING_PAIR_RESPONSE_2_LEN 22

/*!
 * Size of the third pairing request in bytes.
 */
#define UNIFYING_PAIR_REQUEST_3_LEN 22

/*!
 * Size of the third pairing response in bytes.
 */
#define UNIFYING_PAIR_RESPONSE_3_LEN 10

/*!
 * Size of the pairing complete request in bytes.
 * No pairing response is expected.
 */
#define UNIFYING_PAIR_COMPLETE_REQUEST_LEN 10

/*!
 * Size of a large wake-up request in bytes.
 */
#define UNIFYING_LONG_WAKE_UP_REQUEST_LEN 22

/*!
 * Size of a small wake-up request in bytes.
 */
#define UNIFYING_SHORT_WAKE_UP_REQUEST_LEN 10

/*!
 * Size of a set timeout request in bytes.
 */
#define UNIFYING_SET_TIMEOUT_REQUEST_LEN 10

/*!
 * Size of a keep-alive request in bytes.
 */
#define UNIFYING_KEEP_ALIVE_REQUEST_LEN 5

/*!
 * Size of a small HID++ 1.0 payload in bytes.
 */
#define UNIFYING_HIDPP_1_0_SHORT_LEN 10

/*!
 * Number of parameters that a small HID++ 1.0 payload can hold.
 */
#define UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN 4

/*!
 * Size of a large HID++ 1.0 payload in bytes.
 */
#define UNIFYING_HIDPP_1_0_LONG_LEN 22

/*!
 * Number of parameters that a small HID++ 1.0 payload can hold.
 */
#define UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN 17

/*!
 * HID++ 1.0 `SET_REGISTER` SubID.
 */
#define UNIFYING_HIDPP_1_0_SUB_ID_SET_REGISTER 0x80

/*!
 * HID++ 1.0 `GET_REGISTER` SubID.
 */
#define UNIFYING_HIDPP_1_0_SUB_ID_GET_REGISTER 0x81

/*!
 * HID++ 1.0 `SET_LONG_REGISTER` SubID.
 */
#define UNIFYING_HIDPP_1_0_SUB_ID_SET_LONG_REGISTER 0x82

/*!
 * HID++ 1.0 `GET_LONG_REGISTER` SubID.
 */
#define UNIFYING_HIDPP_1_0_SUB_ID_GET_LONG_REGISTER 0x83

/*!
 * HID++ 1.0 `ERROR_MSG` SubID.
 */
#define UNIFYING_HIDPP_1_0_SUB_ID_ERROR_MSG 0x8F

/*!
 * HID++ 1.0 `SUCCESS` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_SUCCESS 0x00

/*!
 * HID++ 1.0 `INVALID_SUBID` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_SUBID 0x01

/*!
 * HID++ 1.0 `INVALID_ADDRESS` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_ADDRESS 0x02

/*!
 * HID++ 1.0 `INVALID_VALUE` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_VALUE 0x03

/*!
 * HID++ 1.0 `CONNECT_FAIL` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_CONNECT_FAIL 0x04

/*!
 * HID++ 1.0 `TOO_MANY_DEVICES` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_TOO_MANY_DEVICES 0x05

/*!
 * HID++ 1.0 `ALREADY_EXISTS` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_ALREADY_EXISTS 0x06

/*!
 * HID++ 1.0 `BUSY` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_BUSY 0x07

/*!
 * HID++ 1.0 `UNKNOWN_DEVICE` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_UNKNOWN_DEVICE 0x08

/*!
 * HID++ 1.0 `RESOURCE_ERROR` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_RESOURCE_ERROR 0x09

/*!
 * HID++ 1.0 `REQUEST_UNAVAILABLE` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_REQUEST_UNAVAILABLE 0x0A

/*!
 * HID++ 1.0 `INVALID_PARAM_VALUE` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_PARAM_VALUE 0x0B

/*!
 * HID++ 1.0 `WRONG_PIN_CODE` error code.
 */
#define UNIFYING_HIDPP_1_0_ERROR_WRONG_PIN_CODE 0x0C

// These are the only supported default timeouts according to the HID++ 1.0 specification.
/*!
 * Default HID++ timeout for keyboards.
 * \see UNIFYING_DEFAULT_TIMEOUT_MOUSE
 */
#define UNIFYING_DEFAULT_TIMEOUT_KEYBOARD 20

/*!
 * Default HID++ timeout for mice.
 * \see UNIFYING_DEFAULT_TIMEOUT_KEYBOARD
 */
#define UNIFYING_DEFAULT_TIMEOUT_MOUSE 8

/*!
 * Size of an encrypted keystroke request in bytes.
 */
#define UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN 22

/*!
 * Size of an unencrypted keystroke request in bytes.
 * \todo    check if this is still supported.
 */
#define UNIFYING_UNENCRYPTED_KEYSTROKE_REQUEST_LEN 10

/*!
 * Size of a multimedia keystroke request in bytes.
 */
#define UNIFYING_MULTIMEDIA_KEYSTROKE_REQUEST_LEN 10

/*!
 * Size of a mouse request in bytes.
 */
#define UNIFYING_MOUSE_REQUEST_LEN 10

/*!
 * Bitmask for de-obfuscate an AES key.
 * 
 * \see unifying_deobfuscate_aes_key()
 */
extern const uint8_t unifying_aes_key_bitmask[UNIFYING_AES_BLOCK_LEN];

/*!
 * AES key byte indices for de-obfuscate an AES key.
 * 
 * The On-Line Encyclopedia of Integer Sequences returns no matches for this sequence nor any similar sequence.
 * It appears to be random.
 * 
 * \see unifying_deobfuscate_aes_key()
 */
extern const uint8_t unifying_aes_key_index[UNIFYING_AES_BLOCK_LEN];

/*!
 * AES nonce that prefixes the AES counter in the AES initialization vector (IV).
 */
extern const uint8_t unifying_aes_nonce_prefix[UNIFYING_AES_NONCE_PREFIX_LEN];

/*!
 * AES nonce that suffixes the AES counter in the AES initialization vector (IV).
 */
extern const uint8_t unifying_aes_nonce_suffix[UNIFYING_AES_NONCE_SUFFIX_LEN];

/*!
 * All RF channels that a Unifying receiver listens on during normal operation.
 */
extern const uint8_t unifying_channels[UNIFYING_CHANNELS_LEN];

/*!
 * All RF channels that a Unifying receiver listens on during pairing.
 * 
 */
extern const uint8_t unifying_pairing_channels[UNIFYING_PAIRING_CHANNELS_LEN];

/*!
 * Initial RF address for pairing.
 */
extern const uint8_t unifying_pairing_address[UNIFYING_ADDRESS_LEN];

#endif
