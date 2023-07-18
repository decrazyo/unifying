
/*!
 * \file unifying_utils.h
 * \brief Miscellaneous utility functions used by the Unifying protocol
 */

#ifndef UNIFYING_UTILS_H
#define UNIFYING_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "unifying_const.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Pack a 16-bit integer into a byte array.
 * 
 * Bytes are packed into the array with the most significant byte first.
 * 
 * \param[out]  packed  Pointer to byte array that is at least 2 bytes long.
 * \param[in]   number  A 16-bit integer to pack.
 */
void unifying_uint16_pack(uint8_t packed[2], uint16_t number);

/*!
 * Unpack a byte array into a 16-bit integer.
 * 
 * Array bytes are expected to have the most significant byte first.
 * 
 * \param[out]  number  Pointer to a 16-bit integer to unpack into.
 * \param[in]   packed  Pointer to byte array that is at least 2 bytes long.
 */
void unifying_uint16_unpack(uint16_t* number, const uint8_t packed[2]);

/*!
 * Pack a 32-bit integer into a byte array.
 * 
 * Bytes are packed into the array with the most significant byte first.
 * 
 * \param[out]  packed  Pointer to byte array that is at least 4 bytes long.
 * \param[in]   number  A 32-bit integer to pack.
 */
void unifying_uint32_pack(uint8_t packed[4], uint32_t number);

/*!
 * Unpack a byte array into a 32-bit integer.
 * 
 * Array bytes are expected to have the most significant byte first.
 * 
 * \param[out]  number  Pointer to a 32-bit integer to unpack into.
 * \param[in]   packed  Pointer to byte array that is at least 4 bytes long.
 */
void unifying_uint32_unpack(uint32_t* number, const uint8_t packed[4]);

/*!
 * Clamp a 16-bit integer to a 12-bit range.
 * 
 * \param[in]   number  A 16-bit integer to clamp.
 * 
 * \return A 16-bit integer with its value clamped to a 12-bit range.
 * 
 * \see unifying_mouse_move_request_pack()
 */
int16_t unifying_int12_clamp(int16_t number);

/*!
 * Compute the checksum of a byte array.
 * 
 * \param[in]   buffer  Pointer to a byte array to compute the checksum for.
 * \param[in]   length  Length of the byte array.
 * 
 * \return The checksum of \p length bytes in \p buffer.
 */
uint8_t unifying_checksum(const uint8_t* buffer, uint8_t length);

/*!
 * Verify the checksum of a byte array.
 * 
 * Compute the checksum of the first \p length `- 1` bytes of \p buffer and compare it to the last byte of \p buffer.
 * 
 * \param[in]   buffer  Pointer to a byte array to verify.
 * \param[in]   length  Length of the byte array.
 * 
 * \return `0` if the checksum is correct.
 * \return `1` if the checksum is incorrect.
 * 
 * \see unifying_checksum()
 */
uint8_t unifying_checksum_verify(const uint8_t* buffer, uint8_t length);

/*!
 * Perform a bitwise XNOR on 2 bytes and return the result.
 * 
 * \param[in]   first   A byte.
 * \param[in]   second  Another byte.
 * 
 * \return The bitwise XOR of \p first and \p second followed by a bitwise negation.
 * 
 * \see unifying_deobfuscate_aes_key()
 */
uint8_t unifying_xnor(uint8_t first, uint8_t second);

/*!
 * De-obfuscate a Logitech Unifying AES key.
 * 
 * \param[out]  aes_key         Buffer of at least \ref UNIFYING_AES_BLOCK_LEN bytes to store the de-obfuscate AES key.
 * \param[in]   proto_aes_key   At least \ref UNIFYING_AES_BLOCK_LEN bytes of obfuscated AES key data.
 * 
 * \see unifying_proto_aes_key
 * \see unifying_proto_aes_key_init()
 * \see unifying_proto_aes_key_pack()
 * \see unifying_aes_key_bitmask
 * \see unifying_xnor()
 * \see unifying_aes_key_index
 */
void unifying_deobfuscate_aes_key(uint8_t aes_key[UNIFYING_AES_BLOCK_LEN],
                                  const uint8_t proto_aes_key[UNIFYING_AES_BLOCK_LEN]);

/*!
 * Compute the next RF channel to use if a transmission fails.
 * 
 * \param[in]   current_channel     The current RF channel.
 * 
 * \return  A new RF channel to use.
 * 
 * \see unifying_channels
 */
uint8_t unifying_next_channel(uint8_t current_channel);

/*!
 * Reverse the order of a byte array.
 * 
 * This function is provided purely for the convenience of this library's user.
 * This library stores RF addresses with the most significant byte first
 * but nRF24 radios expect addresses to be specified with the least significant byte first.
 * Since this library's user is expected to write functions for interfacing with nRF hardware,
 * they may need a way to reverse the byte order of an RF addresses.
 * 
 * \param[out]  reverse     A buffer that is at least \p length bytes long to contain the reverse of \p forward.
 * \param[in]   forward     A buffer of at least \p length bytes to reverse.
 * \param[in]   length      Size of \p reverse and \p forward.
 */
void unifying_copy_reverse(uint8_t *reverse, const uint8_t *forward, uint8_t length);

/*!
 * Print a buffer to stdout.
 * 
 * This function is provided for debugging purposes.
 * 
 * \param[in]   buffer  A buffer of at least \p length bytes to print.
 * \param[in]   length  Size of \p buffer.
 */
void unifying_print_buffer(const uint8_t *buffer, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
