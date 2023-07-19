
#include "unifying_utils.h"

void unifying_uint16_pack(uint8_t packed[2], uint16_t number)
{
    packed[0] = (number >> 8) & 0xFF;
    packed[1] = (number >> 0) & 0xFF;
}

void unifying_uint16_unpack(uint16_t* number, const uint8_t packed[2])
{
    for(size_t i = 0; i < sizeof(*number); ++i)
    {
        *number <<= 8;
        *number |= packed[i];
    }
}

void unifying_uint32_pack(uint8_t packed[4], uint32_t number)
{
    packed[0] = (number >> 24) & 0xFF;
    packed[1] = (number >> 16) & 0xFF;
    packed[2] = (number >> 8) & 0xFF;
    packed[3] = (number >> 0) & 0xFF;
}

void unifying_uint32_unpack(uint32_t* number, const uint8_t packed[4])
{
    for(size_t i = 0; i < sizeof(*number); ++i)
    {
        *number <<= 8;
        *number |= packed[i];
    }
}

int16_t unifying_int12_clamp(int16_t number)
{
    if(number > 2047)
    {
        return 2047;
    }

    if(number < -2048)
    {
        return -2048;
    }

    return number;
}

uint8_t unifying_checksum(const uint8_t* buffer, uint8_t length)
{
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        checksum -= buffer[i];
    }
    return checksum;
}

uint8_t unifying_checksum_verify(const uint8_t* buffer, uint8_t length)
{
    return unifying_checksum(buffer, length - 1) != buffer[length - 1];
}

uint8_t unifying_xnor(uint8_t first, uint8_t second)
{
    return ~(first ^ second);
}

void unifying_encrypted_keystroke_plaintext_init(struct unifying_encrypted_keystroke_plaintext* unpacked,
                                                 uint8_t modifiers,
                                                 const uint8_t keys[UNIFYING_KEYS_LEN])
{
    memset(unpacked, 0, sizeof(struct unifying_encrypted_keystroke_plaintext));
    unpacked->modifiers = modifiers;
    memcpy(unpacked->keys, keys, sizeof(unpacked->keys));
    unpacked->flag = 0xC9;
}

void unifying_encrypted_keystroke_plaintext_pack(uint8_t packed[UNIFYING_AES_DATA_LEN],
                                                 const struct unifying_encrypted_keystroke_plaintext* unpacked)
{
    packed[0] = unpacked->modifiers;
    memcpy(&packed[1], unpacked->keys, sizeof(unpacked->keys));
    packed[7] = unpacked->flag;
}

void unifying_encrypted_keystroke_iv_init(struct unifying_encrypted_keystroke_iv* unpacked,
                                          uint32_t counter)
{
    memset(unpacked, 0, sizeof(struct unifying_encrypted_keystroke_iv));
    memcpy(unpacked->prefix, unifying_aes_nonce_prefix, sizeof(unpacked->prefix));
    unpacked->counter = counter;
    memcpy(unpacked->suffix, unifying_aes_nonce_suffix, sizeof(unpacked->suffix));
}

void unifying_encrypted_keystroke_iv_pack(uint8_t packed[UNIFYING_AES_BLOCK_LEN],
                                          const struct unifying_encrypted_keystroke_iv* unpacked)
{
    memcpy(&packed[0], unpacked->prefix, sizeof(unpacked->prefix));
    unifying_uint32_pack(&packed[7], unpacked->counter);
    memcpy(&packed[11], unpacked->suffix, sizeof(unpacked->suffix));
}

void unifying_proto_aes_key_init(struct unifying_proto_aes_key* unpacked,
                                     uint8_t base_address[UNIFYING_ADDRESS_LEN - 1],
                                     uint16_t device_product_id,
                                     uint16_t receiver_product_id,
                                     uint32_t device_crypto,
                                     uint32_t receiver_crypto)
{
    memcpy(unpacked->base_address, base_address, sizeof(unpacked->base_address));
    unpacked->device_product_id = device_product_id;
    unpacked->receiver_product_id = receiver_product_id;
    unpacked->device_crypto = device_crypto;
    unpacked->receiver_crypto = receiver_crypto;
}

void unifying_proto_aes_key_pack(uint8_t packed[UNIFYING_AES_BLOCK_LEN],
                                 const struct unifying_proto_aes_key* unpacked)
{
    memcpy(&packed[0], unpacked->base_address, sizeof(unpacked->base_address));
    unifying_uint16_pack(&packed[4], unpacked->device_product_id);
    unifying_uint16_pack(&packed[6], unpacked->receiver_product_id);
    unifying_uint32_pack(&packed[8], unpacked->device_crypto);
    unifying_uint32_pack(&packed[12], unpacked->receiver_crypto);
}

void unifying_deobfuscate_aes_key(uint8_t aes_key[UNIFYING_AES_BLOCK_LEN],
                                  const uint8_t proto_aes_key[UNIFYING_AES_BLOCK_LEN])
{
    for (uint8_t i = 0; i < UNIFYING_AES_BLOCK_LEN; i++) {
        aes_key[i] = unifying_xnor(proto_aes_key[unifying_aes_key_index[i]], unifying_aes_key_bitmask[i]);
    }
}

uint8_t unifying_next_channel(uint8_t channel)
{
    // Convert the supplied channel into an index into an array of valid channels.
    // This is excessive but it should ensure that we always return a valid channel.
    uint8_t index = (channel - 2) / 3;

    if(index >= UNIFYING_CHANNELS_LEN)
    {
        index = 0;
    }

    return unifying_channels[index];
}

void unifying_copy_reverse(uint8_t *reverse, const uint8_t *forward, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        reverse[i] = forward[length - i - 1];
    }
}

void unifying_print_buffer(const uint8_t *buffer, uint8_t length)
{
    printf("[");
    for(uint8_t i = 0; i < length; i++)
    {
        if(i)
        {
            printf(", ");
        }
        printf("0x%02X", buffer[i]);
    }
    printf("]\n");
}

