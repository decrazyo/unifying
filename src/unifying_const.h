
#ifndef UNIFYING_CONST_H
#define UNIFYING_CONST_H

#include <stdint.h>

#ifndef UNIFYING_TIMEOUT_COEFFICIENT
#define UNIFYING_TIMEOUT_COEFFICIENT 0.875
#endif

#define UNIFYING_AES_BLOCK_LEN 16
#define UNIFYING_AES_NONCE_PREFIX_LEN 7
#define UNIFYING_AES_NONCE_SUFFIX_LEN 5
#define UNIFYING_AES_DATA_LEN 8

#define UNIFYING_CHANNELS_LEN 25
#define UNIFYING_PAIRING_CHANNELS_LEN 11
#define UNIFYING_ADDRESS_LEN 5

#define UNIFYING_MAX_PAYLOAD_LEN 22
#define UNIFYING_MAX_NAME_LEN 16
#define UNIFYING_KEYS_LEN 6

#define UNIFYING_PAIR_REQUEST_1_LEN 22
#define UNIFYING_PAIR_RESPONSE_1_LEN 22
#define UNIFYING_PAIR_REQUEST_2_LEN 22
#define UNIFYING_PAIR_RESPONSE_2_LEN 22
#define UNIFYING_PAIR_REQUEST_3_LEN 22
#define UNIFYING_PAIR_RESPONSE_3_LEN 10
#define UNIFYING_PAIR_COMPLETE_REQUEST_LEN 10

#define UNIFYING_WAKE_UP_REQUEST_1_LEN 22
#define UNIFYING_WAKE_UP_REQUEST_2_LEN 10

#define UNIFYING_SET_TIMEOUT_REQUEST_LEN 10
#define UNIFYING_KEEP_ALIVE_REQUEST_LEN 5

#define UNIFYING_HIDPP_1_0_SHORT_LEN 10
#define UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN 4

#define UNIFYING_HIDPP_1_0_LONG_LEN 22
#define UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN 17

#define UNIFYING_HIDPP_1_0_SUB_ID_SET_REGISTER 0x80
#define UNIFYING_HIDPP_1_0_SUB_ID_GET_REGISTER 0x81
#define UNIFYING_HIDPP_1_0_SUB_ID_SET_LONG_REGISTER 0x82
#define UNIFYING_HIDPP_1_0_SUB_ID_GET_LONG_REGISTER 0x83
#define UNIFYING_HIDPP_1_0_SUB_ID_ERROR_MSG 0x8F

#define UNIFYING_HIDPP_1_0_ERROR_SUCCESS 0x00
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_SUBID 0x01
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_ADDRESS 0x02
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_VALUE 0x03
#define UNIFYING_HIDPP_1_0_ERROR_CONNECT_FAIL 0x04
#define UNIFYING_HIDPP_1_0_ERROR_TOO_MANY_DEVICES 0x05
#define UNIFYING_HIDPP_1_0_ERROR_ALREADY_EXISTS 0x06
#define UNIFYING_HIDPP_1_0_ERROR_BUSY 0x07
#define UNIFYING_HIDPP_1_0_ERROR_UNKNOWN_DEVICE 0x08
#define UNIFYING_HIDPP_1_0_ERROR_RESOURCE_ERROR 0x09
#define UNIFYING_HIDPP_1_0_ERROR_REQUEST_UNAVAILABLE 0x0A
#define UNIFYING_HIDPP_1_0_ERROR_INVALID_PARAM_VALUE 0x0B
#define UNIFYING_HIDPP_1_0_ERROR_WRONG_PIN_CODE 0x0C

// These are the only supported default timeouts according to the HID++ 1.0 specification.
#define UNIFYING_DEFAULT_TIMEOUT_KEYBOARD 20
#define UNIFYING_DEFAULT_TIMEOUT_MOUSE 8

#define UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN 22
#define UNIFYING_UNENCRYPTED_KEYSTROKE_REQUEST_LEN 10
#define UNIFYING_MULTIMEDIA_KEYSTROKE_REQUEST_LEN 10
#define UNIFYING_MOUSE_MOVE_REQUEST_LEN 10

extern const uint8_t unifying_aes_key_bitmask[UNIFYING_AES_BLOCK_LEN];
extern const uint8_t unifying_aes_key_index[UNIFYING_AES_BLOCK_LEN];
extern const uint8_t unifying_aes_nonce_prefix[UNIFYING_AES_NONCE_PREFIX_LEN];
extern const uint8_t unifying_aes_nonce_suffix[UNIFYING_AES_NONCE_SUFFIX_LEN];
extern const uint8_t unifying_channels[UNIFYING_CHANNELS_LEN];
extern const uint8_t unifying_pairing_channels[UNIFYING_PAIRING_CHANNELS_LEN];
extern const uint8_t unifying_pairing_address[UNIFYING_ADDRESS_LEN];

#endif