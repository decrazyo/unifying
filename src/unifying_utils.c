
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

