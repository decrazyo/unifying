
/*!
 * \file unifying_state.h
 * \brief Structures and functions for managing the Unifying protocol's state.
 * 
 * \todo: Add functions for allocating and freeing \ref unifying_state
 */

#ifndef UNIFYING_STATE_H
#define UNIFYING_STATE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "unifying_const.h"
#include "unifying_buffer.h"

/*!
 * Functions for interfacing with hardware.
 * 
 * These functions are expected to be defined by the user of this library.
 * 
 * \note    The functionality of payload_available and payload_size may be combined in the future.
 */
struct unifying_interface
{
    /*!
     * Transmit an RF payload with an nRF24 compatible radio.
     * 
     * \param[in]   payload     Payload data to transmit.
     * \param[in]   length      Length of the payload data.
     * 
     * \return  `0` if successful.
     * \return  Anything else on failure.
     */
    uint8_t (*transmit_payload)(const uint8_t* payload, uint8_t length);
    /*!
     * Receive an RF payload with an nRF24 compatible radio.
     * 
     * \param[out]  payload     Buffer for returning the received payload.
     * \param[in]   length      Length of the payload buffer.
     * 
     * \return  The length of the received payload.
     * \return  `0` if no payload is available.
     */
    uint8_t (*receive_payload)(uint8_t* payload, uint8_t length);
    /*!
     * Indicate if an RF payload is available to be received.
     * 
     * \return  `true` if an RF payload is available.
     * \return  `false` if no RF payload is available.
     */
    bool (*payload_available)();
    /*!
     * Return the size of the most recently received payload.
     * 
     * \return  Size of the most recently received payload.
     */
    uint8_t (*payload_size)();
    /*!
     * Set the address that the radio transmits and receives on.
     * 
     * \param[in]   address     Address to set the radio to.
     * 
     * \return  `0` if successful.
     * \return  Anything else on failure.
     */
    uint8_t (*set_address)(const uint8_t address[UNIFYING_ADDRESS_LEN]);
    /*!
     * Set the channel that the radio transmits and receives on.
     * 
     * \param[in]   channel     Channel to set the radio to.
     * 
     * \return  `0` if successful.
     * \return  Anything else on failure.
     */
    uint8_t (*set_channel)(uint8_t channel);
    /*!
     * Return the time in milliseconds since execution started.
     * 
     * \todo    Use millis() by default if we are running on Arduino.
     * 
     * \return  Time in milliseconds since execution started.
     */
    uint32_t (*time)();
    /*!
     * AES-128 encrypt the supplied data.
     * 
     * This function is provided so that hardware-accelerated AES can be used if it is available.
     * 
     * \todo    Add a default implementation that leverages Tiny AES.
     *          https://github.com/kokke/tiny-AES-c
     * 
     * \param[in,out]   data    \ref UNIFYING_AES_DATA_LEN bytes of unencrypted data are supplied.
     *                          If encryption is successful then at least \ref UNIFYING_AES_DATA_LEN bytes
     *                          of encrypted data should be returned.
     * \param[in]       key     AES-128 encryption key.
     * \param[in]       iv      AES-128 initialization vector.
     * 
     * \return  `0` if successful.
     * \return  Anything else on failure.
     */
    uint8_t (*encrypt)(uint8_t data[UNIFYING_AES_DATA_LEN],
                       const uint8_t key[UNIFYING_AES_BLOCK_LEN],
                       const uint8_t iv[UNIFYING_AES_BLOCK_LEN]);
};

/*!
 * State information that is required for the Unifying protocol to operate correctly.
 */
struct unifying_state
{
    /// Functions for interfacing with hardware.
    const struct unifying_interface* interface;
    /// Buffer for payloads to be transmitted.
    struct unifying_ring_buffer* transmit_buffer;
    /// Buffer for received payloads to be handled.
    struct unifying_ring_buffer* receive_buffer;
    /// RF address.
    uint8_t *address;
    /// AES-128 encryption key.
    uint8_t *aes_key;
    /// AES counter.
    uint32_t aes_counter;
    /*!
     * Default timeout.
     * Transmitting some payloads will set `timeout` to this value.
     */
    uint16_t default_timeout;
    /// Current timeout for keep-alive packets.
    uint16_t timeout;
    /// Time that the previous payload was transmitted.
    uint32_t previous_transmit;
    /// Time that the next payload should be transmitted, based on the current timeout.
    uint32_t next_transmit;
    /// Current RF channel. This is used to compute a new channel in the event of a transmission failure.
    uint8_t channel;
};

/*!
 * Information stored in \ref unifying_state.transmit_buffer "state.transmit_buffer"
 */
struct unifying_transmit_entry
{
    uint8_t* payload;
    uint8_t length;
    uint8_t timeout;
};

/*!
 * Information stored in \ref unifying_state.receive_buffer "state.receive_buffer"
 */
struct unifying_receive_entry
{
    uint8_t* payload;
    uint8_t length;
};

#ifdef __cplusplus
extern "C" {
#endif


void unifying_interface_init(struct unifying_interface* interface,
                             uint8_t (*transmit_payload)(const uint8_t* payload, uint8_t length),
                             uint8_t (*receive_payload)(uint8_t* payload, uint8_t length),
                             bool (*payload_available)(),
                             uint8_t (*payload_size)(),
                             uint8_t (*set_address)(const uint8_t address[UNIFYING_ADDRESS_LEN]),
                             uint8_t (*set_channel)(uint8_t channel),
                             uint32_t (*time)(),
                             uint8_t (*encrypt)(uint8_t data[UNIFYING_AES_DATA_LEN],
                                                const uint8_t key[UNIFYING_AES_BLOCK_LEN],
                                                const uint8_t iv[UNIFYING_AES_BLOCK_LEN]));

void unifying_state_init(struct unifying_state* state,
                         const struct unifying_interface* interface,
                         struct unifying_ring_buffer* transmit_buffer,
                         struct unifying_ring_buffer* receive_buffer,
                         uint8_t address[UNIFYING_ADDRESS_LEN],
                         uint8_t aes_key[UNIFYING_AES_BLOCK_LEN],
                         uint32_t aes_counter,
                         uint16_t default_timeout,
                         uint8_t channel);

void unifying_state_transmit_buffer_clear(struct unifying_state* state);
void unifying_state_receive_buffer_clear(struct unifying_state* state);
void unifying_state_buffers_clear(struct unifying_state* state);

void unifying_state_channel_set(struct unifying_state* state, uint8_t channel);

void unifying_state_address_set(struct unifying_state* state, const uint8_t address[UNIFYING_ADDRESS_LEN]);

void unifying_transmit_entry_init(struct unifying_transmit_entry* entry,
                                  uint8_t* payload,
                                  uint8_t length,
                                  uint8_t timeout);
struct unifying_transmit_entry* unifying_transmit_entry_create(uint8_t length, uint8_t timeout);
void unifying_transmit_entry_destroy(struct unifying_transmit_entry* entry);

void unifying_receive_entry_init(struct unifying_receive_entry* entry,
                                         uint8_t* payload,
                                         uint8_t length);
struct unifying_receive_entry* unifying_receive_entry_create(uint8_t length);
void unifying_receive_entry_destroy(struct unifying_receive_entry* entry);


#ifdef __cplusplus
}
#endif

#endif
