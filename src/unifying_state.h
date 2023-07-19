
/*!
 * \file unifying_state.h
 * \brief Structures and functions for managing the Unifying protocol's state.
 * 
 * \todo Add functions for allocating and freeing \ref unifying_state
 * 
 * \todo Add a function for setting timeout state.
 */

#ifndef UNIFYING_STATE_H
#define UNIFYING_STATE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "unifying_const.h"
#include "unifying_error.h"
#include "unifying_buffer.h"

/*!
 * Compile and use a software implementation of AES encryption by default.
 * 
 * Defining this as anything other than `0` will disable the default implementation.
 * If the default implementation is disable then this library's user is expected to provide their own implementation.
 */
#ifndef UNIFYING_HARDWARE_AES
#define UNIFYING_HARDWARE_AES 0
#endif

#if defined(UNIFYING_HARDWARE_AES) && (UNIFYING_HARDWARE_AES == 0)
// https://github.com/kokke/tiny-AES-c
#include "aes.h"
#endif

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
    /// Array of bytes to transmit.
    uint8_t* payload;
    /// Size of `payload` in bytes.
    uint8_t length;
    /// New timeout value to set if `payload` is successfully transmitted.
    uint8_t timeout;
};

/*!
 * Information stored in \ref unifying_state.receive_buffer "state.receive_buffer"
 */
struct unifying_receive_entry
{
    /// Array of received bytes.
    uint8_t* payload;
    /// Size of `payload` in bytes.
    uint8_t length;
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Initialize a \ref unifying_interface structure.
 * 
 * \param[out]  interface           A \ref unifying_interface to initialize.
 * \param[in]   transmit_payload    Function for transmitting RF payloads.
 *                                  see \ref unifying_interface.transmit_payload for more details.
 * \param[in]   receive_payload     Function for receiving RF payloads.
 *                                  see \ref unifying_interface.receive_payload for more details.
 * \param[in]   payload_available   Function for determining if an RF payload is available to be read.
 *                                  see \ref unifying_interface.payload_available for more details.
 * \param[in]   payload_size        Function for determining the size of an available RF payload.
 *                                  see \ref unifying_interface.payload_size for more details.
 * \param[in]   set_address         Function for setting the RF address of a radio.
 *                                  see \ref unifying_interface.set_address for more details.
 * \param[in]   set_channel         Function for setting the RF channel of a radio.
 *                                  see \ref unifying_interface.set_channel for more details.
 * \param[in]   time                Function getting the time since execution started in milliseconds.
 *                                  see \ref unifying_interface.time for more details.
 * \param[in]   encrypt             Function for AES-128 encrypting data.
 *                                  If \ref UNIFYING_HARDWARE_AES is `0` (default) then 
 *                                  a default implementation is provided for this function.
 *                                  Specify `NULL` to use the default implementation.
 *                                  see \ref unifying_interface.encrypt for more details.
 * 
 * \return  \ref UNIFYING_ERROR if \p encrypt is `NULL` and no default implementation is available.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 * 
 * \see unifying_interface
 */
enum unifying_error unifying_interface_init(struct unifying_interface* interface,
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

/*!
 * Initialize a \ref unifying_state structure.
 * 
 * \param[out]  state               Pointer to a \ref unifying_state to initialize.
 * \param[in]   interface           Pointer to an initialized \ref unifying_interface
 *                                  for accessing hardware features.
 * \param[in]   transmit_buffer     Pointer to an initialized \ref unifying_ring_buffer 
 *                                  for buffering payloads for transmission.
 * \param[in]   receive_buffer      Pointer to an initialized \ref unifying_ring_buffer 
 *                                  for buffering received payloads.
 * \param[in]   address             Byte array with space for at least \ref UNIFYING_ADDRESS_LEN bytes
 *                                  to store an RF address.
 * \param[in]   aes_key             Byte array with space for at least \ref UNIFYING_AES_BLOCK_LEN bytes
 *                                  to store an AES encryption key.
 * \param[in]   aes_counter         A random 32-bit integer for AES encryption.
 * \param[in]   default_timeout     Default timeout used by some payloads.
 * \param[in]   channel             RF channel to communicate on.
 *                                  This should be a value from \ref unifying_channels.
 * 
 * \todo    Define valid values for `default_timeout`.
 * 
 * \see unifying_state
 * \see unifying_channels
 */
void unifying_state_init(struct unifying_state* state,
                         const struct unifying_interface* interface,
                         struct unifying_ring_buffer* transmit_buffer,
                         struct unifying_ring_buffer* receive_buffer,
                         uint8_t address[UNIFYING_ADDRESS_LEN],
                         uint8_t aes_key[UNIFYING_AES_BLOCK_LEN],
                         uint32_t aes_counter,
                         uint16_t default_timeout,
                         uint8_t channel);

/*!
 * Remove and free all items in \ref unifying_state.transmit_buffer "state.transmit_buffer".
 * 
 * \param[in,out]   state   Unifying state information.
 */
void unifying_state_transmit_buffer_clear(struct unifying_state* state);

/*!
 * Remove and free all items in \ref unifying_state.receive_buffer "state.receive_buffer".
 * 
 * \param[in,out]   state   Unifying state information.
 */
void unifying_state_receive_buffer_clear(struct unifying_state* state);

/*!
 * Remove and free all items in \ref unifying_state.transmit_buffer "state.transmit_buffer" 
 * and \ref unifying_state.receive_buffer "state.receive_buffer".
 * 
 * \param[in,out]   state   Unifying state information.
 */
void unifying_state_buffers_clear(struct unifying_state* state);

/*!
 * Set the RF channel.
 * 
 * Calls \ref unifying_interface.set_channel() "state.interface.set_channel()" to change the RF channel.
 * If that's successful then \ref unifying_state.timeout "state.timeout" will be updated as well.
 * 
 * \param[in,out]   state       Unifying state information.
 * \param[in]       channel     New RF channel to use.
 * 
 * \return  The return value of \ref unifying_interface.set_channel() "state.interface.set_channel()".
 */
uint8_t unifying_state_channel_set(struct unifying_state* state, uint8_t channel);

/*!
 * Set the RF address.
 * 
 * Calls \ref unifying_interface.set_address() "state.interface.set_address()" to change the RF address.
 * If that's successful then \ref unifying_state.timeout "state.timeout" will be updated as well.
 * 
 * \param[in,out]   state       Unifying state information.
 * \param[in]       address     New RF address to use.
 * 
 * \return  The return value of \ref unifying_interface.set_address() "state.interface.set_address()".
 */
uint8_t unifying_state_address_set(struct unifying_state* state, const uint8_t address[UNIFYING_ADDRESS_LEN]);

/*!
 * Initialize a \ref unifying_transmit_entry structure.
 * 
 * \param[out]  entry       Pointer to a \ref unifying_transmit_entry to initialize.
 * \param[in]   payload     Byte array of size `length`.
 * \param[in]   length      Size of `payload` in bytes.
 * \param[in]   timeout     A new timeout associated with `payload`.
 * 
 * \see unifying_transmit_entry
 */
void unifying_transmit_entry_init(struct unifying_transmit_entry* entry,
                                  uint8_t* payload,
                                  uint8_t length,
                                  uint8_t timeout);

/*!
 * Create and initialize a \ref unifying_transmit_entry structure.
 * 
 * \param[in]   length      Size in bytes of a buffer to allocate.
 * \param[in]   timeout     A new timeout to be associated with a payload.
 * 
 * \return  `NULL` if allocation fails.
 * \return  Pointer to new \ref unifying_transmit_entry instance otherwise.
 * 
 * \see unifying_transmit_entry
 * \see unifying_transmit_entry_destroy()
 */
struct unifying_transmit_entry* unifying_transmit_entry_create(uint8_t length, uint8_t timeout);

/*!
 * Free a \ref unifying_transmit_entry instance.
 * 
 * \param[in,out]   entry   Pointer to a \ref unifying_transmit_entry to free.
 * 
 * \see unifying_transmit_entry
 * \see unifying_transmit_entry_create()
 */
void unifying_transmit_entry_destroy(struct unifying_transmit_entry* entry);

/*!
 * Initialize a \ref unifying_receive_entry structure.
 * 
 * \param[out]  entry       Pointer to a \ref unifying_receive_entry to initialize.
 * \param[in]   payload     Byte array of size `length`.
 * \param[in]   length      Size of `payload` in bytes.
 * 
 * \see unifying_receive_entry
 */
void unifying_receive_entry_init(struct unifying_receive_entry* entry,
                                         uint8_t* payload,
                                         uint8_t length);

/*!
 * Create and initialize a \ref unifying_receive_entry structure.
 * 
 * \param[in]   length      Size in bytes of a buffer to allocate.
 * 
 * \return  `NULL` if allocation fails.
 * \return  Pointer to new \ref unifying_receive_entry instance otherwise.
 * 
 * \see unifying_receive_entry
 * \see unifying_receive_entry_destroy()
 */
struct unifying_receive_entry* unifying_receive_entry_create(uint8_t length);

/*!
 * Free a \ref unifying_receive_entry instance.
 * 
 * \param[in,out]   entry   Pointer to a \ref unifying_receive_entry to free.
 * 
 * \see unifying_receive_entry
 * \see unifying_receive_entry_create()
 */
void unifying_receive_entry_destroy(struct unifying_receive_entry* entry);


#ifdef __cplusplus
}
#endif

#endif
