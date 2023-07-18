
#include "unifying_const.h"

const uint8_t unifying_aes_key_bitmask[UNIFYING_AES_BLOCK_LEN] = {
    0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xAA, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xAA, 0xFF, 0xFF
};

const uint8_t unifying_aes_key_index[UNIFYING_AES_BLOCK_LEN] = {
    0x07, 0x01, 0x00, 0x03, 0x0A, 0x02, 0x09, 0x0E, 0x08, 0x06, 0x0C, 0x05, 0x0D, 0x0F, 0x04, 0x0B
};

const uint8_t unifying_aes_nonce_prefix[UNIFYING_AES_NONCE_PREFIX_LEN] = {
    0x04, 0x14, 0x1D, 0x1F, 0x27, 0x28, 0x0D
};

const uint8_t unifying_aes_nonce_suffix[UNIFYING_AES_NONCE_SUFFIX_LEN] = {
    0x0A, 0x0D, 0x13, 0x26, 0x0E
};

const uint8_t unifying_channels[UNIFYING_CHANNELS_LEN] = {
    5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47, 50, 53, 56, 59, 62, 65, 68, 71, 74, 77
};

const uint8_t unifying_pairing_channels[UNIFYING_PAIRING_CHANNELS_LEN] = {
    5, 8, 17, 32, 35, 41, 44, 62, 65, 71, 74
};

const uint8_t unifying_pairing_address[UNIFYING_ADDRESS_LEN] = {
    0xBB, 0x0A, 0xDC, 0xA5, 0x75 // MSB first.
};
