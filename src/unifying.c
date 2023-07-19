
#include "unifying.h"

/*!
 * Immediately transmit a payload.
 * 
 * If transmission fails then a new RF channel will be selected and the timeout will not be updated.
 * 
 * \param[in,out]   state       Unifying state information.
 * \param[out]      payload     Pointer to a payload to transmit.
 * \param[in]       length      Length of the payload to transmit.
 * \param[in]       timeout     New timeout for keep alive packets.
 *                              Specifying \ref UNIFYING_TIMEOUT_UNCHANGED will leave the timeout unchanged.
 * 
 * \return  \ref UNIFYING_TRANSMIT_ERROR if transmission failed.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_transmit(struct unifying_state* state,
                                             uint8_t* payload,
                                             uint8_t length,
                                             uint16_t timeout)
{

    uint8_t err = state->interface->transmit_payload(payload, length);

    if(err)
    {
        // Transmission failed.
        // Switch to a new channel.
        unifying_state_channel_set(state, unifying_next_channel(state->channel));
        return UNIFYING_TRANSMIT_ERROR;
    }

    // Transmission succeeded.
    // Adjust the timeout and determine when the next packet should be sent.
    if(timeout)
    {
        state->timeout = timeout;
    }

    state->previous_transmit = state->interface->time();
    state->next_transmit = state->previous_transmit + state->timeout * UNIFYING_TIMEOUT_COEFFICIENT;

    return UNIFYING_SUCCESS;
}

/*!
 * Queue a received payload in a buffer.
 * 
 * \param[in,out]   state   Unifying state information.
 * 
 * \return  \ref UNIFYING_RECEIVE_ERROR if no payload is available.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the receive buffer is full.
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_PAYLOAD_LENGTH_ERROR if the payload's length differs from its expected length.
 *          This should never happen.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_receive(struct unifying_state* state)
{
    // Check if we have received an ACK payload.
    if(!state->interface->payload_available()) {
        return UNIFYING_RECEIVE_ERROR;
    }

    if(unifying_ring_buffer_full(state->receive_buffer))
    {
        // We don't have room to store the payload.
        // Maybe we will later.
        // The payload can just hang out in the radio's RX FIFO in the meantime.
        return UNIFYING_BUFFER_FULL_ERROR;
    }

    uint8_t length = state->interface->payload_size();
    struct unifying_receive_entry* receive_entry;
    receive_entry = unifying_receive_entry_create(length);

    if(!receive_entry)
    {
        // Memory allocation failed.
        return UNIFYING_CREATE_ERROR;
    }

    // Buffer the received payload for now.
    // It will be handled later.
    length = state->interface->receive_payload(receive_entry->payload, receive_entry->length);

    if(length != receive_entry->length)
    {
        // Somehow we received a payload of a different size than was stated earlier.
        // This should never happen.
        unifying_receive_entry_destroy(receive_entry);
        return UNIFYING_PAYLOAD_LENGTH_ERROR;
    }

    if(unifying_ring_buffer_push_back(state->receive_buffer, receive_entry))
    {
        // The buffer didn't have enough space even though we checked it earlier.
        // This should never happen.
        unifying_receive_entry_destroy(receive_entry);
        return UNIFYING_BUFFER_FULL_ERROR;
    }

    return UNIFYING_SUCCESS;
}

/*!
 * Dequeue a received payload and perform basic verification.
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[out]      receive_entry   Pointer to an entry pointer. Used to return the dequeued payload.
 *                                  The pointer is only guaranteed to be valid
 *                                  if this function returns \ref UNIFYING_SUCCESS.
 * \param[in]       length          Expected length of the received payload.
 *                                  Supplying 0 as the payload length will skip the length check.
 * 
 * \return  \ref UNIFYING_BUFFER_EMPTY_ERROR if \ref unifying_state.receive_buffer "state.receive_buffer"
 *          is empty.
 * \return  \ref UNIFYING_CHECKSUM_ERROR if the received payload's computed checksum
 *          does not match its stated checksum.
 * \return  \ref UNIFYING_PAYLOAD_LENGTH_ERROR if the payload's length differs from \p length
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_response(struct unifying_state* state,
                                             struct unifying_receive_entry** receive_entry,
                                             uint8_t length)
{
    *receive_entry = unifying_ring_buffer_pop_front(state->receive_buffer);

    if(!(*receive_entry))
    {
        return UNIFYING_BUFFER_EMPTY_ERROR;
    }

    if(unifying_checksum_verify((*receive_entry)->payload, (*receive_entry)->length))
    {
        unifying_receive_entry_destroy(*receive_entry);
        return UNIFYING_CHECKSUM_ERROR;
    }

    if(length && (*receive_entry)->length != length)
    {
        unifying_receive_entry_destroy(*receive_entry);
        return UNIFYING_PAYLOAD_LENGTH_ERROR;
    }

    return UNIFYING_SUCCESS;
}

/*!
 * Dequeue a response payload and queue a HID++ payload for transmission.
 * 
 * \todo    Refactor this code to be more readable
 * 
 * \todo    Implement proper HID++ responses instead of responding to everything with errors.
 *          - https://drive.google.com/drive/folders/0BxbRzx7vEV7eWmgwazJ3NUFfQ28?resourcekey=0-dQ-Lx1FORQl0KAdOHQaE1A
 *          - https://docs.google.com/document/d/0BxbRzx7vEV7eNDBheWY0UHM5dEU/edit?resourcekey=0-SPDGsNiO52FX6E-mJIXYXQ#!
 *          - https://drive.google.com/file/d/0BxbRzx7vEV7eU3VfMnRuRXktZ3M/view?resourcekey=0-06JzoS5yy_4Asod95f4Ecw
 * 
 * \param[in,out]   state   Unifying state information.
 * 
 * \return  \ref UNIFYING_PAYLOAD_LENGTH_ERROR if the received payload is too short.
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_hidpp_1_0(struct unifying_state* state)
{
    enum unifying_error err;
    struct unifying_receive_entry* receive_entry;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_hidpp_1_0_short hidpp_1_0_short;

    err = unifying_response(state, &receive_entry, 0);

    if(err)
    {
        return err;
    }

    if(receive_entry->length < 4)
    {
        unifying_receive_entry_destroy(receive_entry);
        return UNIFYING_PAYLOAD_LENGTH_ERROR;
    }

    transmit_entry = unifying_transmit_entry_create(UNIFYING_HIDPP_1_0_SHORT_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        unifying_receive_entry_destroy(receive_entry);
        return UNIFYING_CREATE_ERROR;
    }

    receive_entry->payload[5] = UNIFYING_HIDPP_1_0_ERROR_INVALID_SUBID;
    receive_entry->payload[6] = 0x00;
    unifying_hidpp_1_0_short_init(&hidpp_1_0_short,
                                  receive_entry->payload[2], // index
                                  UNIFYING_HIDPP_1_0_SUB_ID_ERROR_MSG,
                                  &receive_entry->payload[3]);
    hidpp_1_0_short.report = 0x50;
    unifying_receive_entry_destroy(receive_entry);
    unifying_hidpp_1_0_short_pack(transmit_entry->payload, &hidpp_1_0_short);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

/*!
 * Queue a payload for step 1 of the pairing process.
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[in]       id              Random value used for verifying the early stage of the pairing process.
 * \param[in]       product_id      Product ID of your device.
 *                                  This value becomes part of the device's AES encryption key.
 *                                  For added security, this should be a cryptographically secure random number.
 * \param[in]       device_type     Values indicating the device type.
 *                                  Valid values and their meaning are not yet documented.
 * 
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_pair_step_1(struct unifying_state* state,
                                                uint8_t id,
                                                uint16_t product_id,
                                                uint16_t device_type)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_pair_request_1 pair_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_PAIR_REQUEST_1_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_CREATE_ERROR;
    }

    unifying_pair_request_1_init(&pair_request, id, state->timeout, product_id, device_type);
    unifying_pair_request_1_pack(transmit_entry->payload, &pair_request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

/*!
 * Queue a payload for step 2 of the pairing process.
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[in]       crypto          A cryptographically secure random number, used for AES encryption key generation.
 * \param[in]       serial          Serial number of your device. The exact value does not matter.
 * \param[in]       capabilities    HID++ capabilities.
 * 
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_pair_step_2(struct unifying_state* state,
                                                uint32_t crypto,
                                                uint32_t serial,
                                                uint16_t capabilities)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_pair_request_2 pair_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_PAIR_REQUEST_2_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_CREATE_ERROR;
    }

    unifying_pair_request_2_init(&pair_request, crypto, serial, capabilities);
    unifying_pair_request_2_pack(transmit_entry->payload, &pair_request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

/*!
 * Queue a payload for step 3 of the pairing process.
 * 
 * \param[in,out]   state           Unifying state information.
 * \param[in]       name            Name of your device.
 *                                  This name will appear in the Logitech Unifying desktop software.
 *                                  This value does not need to be NULL terminated.
 *                                  The name cannot be longer than \ref UNIFYING_MAX_NAME_LEN.
 * \param[in]       name_length     Length of the supplied name.
 *                                  The name length does not include a NULL terminator.
 *                                  The name cannot be longer than \ref UNIFYING_MAX_NAME_LEN.
 * 
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_pair_step_3(struct unifying_state* state,
                                                const char* name,
                                                uint8_t name_length)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_pair_request_3 pair_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_PAIR_REQUEST_3_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_CREATE_ERROR;
    }

    unifying_pair_request_3_init(&pair_request, name, name_length);
    unifying_pair_request_3_pack(transmit_entry->payload, &pair_request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

/*!
 * Queue a payload to complete the pairing process.
 * 
 * \param[in,out]   state           Unifying state information.
 * 
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_pair_complete(struct unifying_state* state)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_pair_complete_request pair_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_PAIR_COMPLETE_REQUEST_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_CREATE_ERROR;
    }

    unifying_pair_complete_request_init(&pair_request);
    unifying_pair_complete_request_pack(transmit_entry->payload, &pair_request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

/*!
 * Queue a keep-alive payload.
 * 
 * \todo remove \p timeout and use \ref unifying_state.timeout "state.timeout" instead.
 * 
 * \param[in,out]   state       Unifying state information.
 * \param[in]       timeout     Current packet timeout.
 * 
 * \return  \ref UNIFYING_CREATE_ERROR if dynamic memory allocation fails.
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the transmit buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
static enum unifying_error unifying_keep_alive(struct unifying_state* state, uint16_t timeout)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_keep_alive_request keep_alive_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_KEEP_ALIVE_REQUEST_LEN, 0);

    if(!transmit_entry)
    {
        return UNIFYING_CREATE_ERROR;
    }

    unifying_keep_alive_request_init(&keep_alive_request, timeout);
    unifying_keep_alive_request_pack(transmit_entry->payload, &keep_alive_request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

enum unifying_error unifying_tick(struct unifying_state* state)
{
    uint32_t current_time = state->interface->time();

    // Handle edge case where next_transmit has overflowed but the current_time hasn't.
    if(state->previous_transmit > state->next_transmit && current_time > state->previous_transmit)
    {
        return UNIFYING_SUCCESS;
    }

    if((current_time >= state->next_transmit) ||
       // Handle edge case where current_time has overflowed but the next_transmit hasn't.
       (state->previous_transmit > current_time && state->next_transmit > current_time))
    {

        if(!unifying_ring_buffer_empty(state->receive_buffer))
        {
            // We have received a payload that hasn't been handled yet.
            // It should be a HID++ query so we'll queue a HID++ response.
            // TODO: Consider handling HID++ queries outside of the transmit interval.
            unifying_hidpp_1_0(state);
        }
        else if(unifying_ring_buffer_empty(state->transmit_buffer))
        {
            // No payloads are queued for transmission so we'll queue a keep alive packet.
            unifying_keep_alive(state, state->timeout);
        }

        // Get a payload and transmit it
        struct unifying_transmit_entry* transmit_entry;
        transmit_entry = unifying_ring_buffer_peek_front(state->transmit_buffer);

        if(!transmit_entry)
        {
            // The buffer appears to be empty even though we checked and queued data if it was empty.
            // This should never happen.
            return UNIFYING_BUFFER_EMPTY_ERROR;
        }

        enum unifying_error err;
        err = unifying_transmit(state,
                                transmit_entry->payload,
                                transmit_entry->length,
                                transmit_entry->timeout);

        if(err)
        {
            // Transmission failed.
            // Keep the payload queued for re-transmission.
            return err;
        }

        // Dequeue and destroy the transmit entry since we won't need it anymore.
        unifying_transmit_entry_destroy(unifying_ring_buffer_pop_front(state->transmit_buffer));

        if(state->interface->payload_available()) {
            return unifying_receive(state);
        }
    }

    return UNIFYING_SUCCESS;
}

enum unifying_error unifying_loop(struct unifying_state* state,
                                  bool exit_on_error,
                                  bool exit_on_transmit,
                                  bool exit_on_receive)
{
    enum unifying_error err = UNIFYING_SUCCESS;

    while(true)
    {
        if(exit_on_error && err)
        {
            // Some error occurred.
            break;
        }

        if(exit_on_transmit && unifying_ring_buffer_empty(state->transmit_buffer))
        {
            // Transmit buffer empty.
            break;
        }

        if(exit_on_receive && !unifying_ring_buffer_empty(state->receive_buffer))
        {
            // Payload received.
            break;
        }

        err = unifying_tick(state);
    }

    return err;
}

enum unifying_error unifying_pair(struct unifying_state* state,
                                  uint8_t id,
                                  uint16_t product_id,
                                  uint16_t device_type,
                                  uint32_t crypto,
                                  uint32_t serial,
                                  uint16_t capabilities,
                                  const char* name,
                                  uint8_t name_length)
{
    enum unifying_error err = UNIFYING_SUCCESS;
    struct unifying_receive_entry* receive_entry;

    // Unifying appears to only support 16 character names.
    if(name_length > UNIFYING_MAX_NAME_LEN)
    {
        return UNIFYING_NAME_LENGTH_ERROR;
    }

    // Pairing begins on a predetermined address.
    if(state->interface->set_address(unifying_pairing_address))
    {
        return UNIFYING_SET_ADDRESS_ERROR;
    }

    // We want total control of the buffers so we'll clear them before pairing.
    unifying_state_buffers_clear(state);

    // Queue a pairing packet for transmission.
    unifying_pair_step_1(state, id, product_id, device_type);

    // We don't know which channel the receiver is listening on.
    // Try to pair on each channel until one works.
    for(int i = 0; i < UNIFYING_CHANNELS_LEN; i++)
    {
        // Transmit the initial paring request.
        err = unifying_loop(state, true, true, false);

        // If transmission fails then we'll try again on another channel.
        if(!err)
        {
            // Success.
            // Continue pairing.
            break;
        }
    }

    // We may have already received a payload from a previous pairing attempt.
    // That payload is invalid so we'll just ignore it.
    unifying_state_buffers_clear(state);

    if(err)
    {
        return err;
    }


    // Send a keep alive payload so we can receive a response.
    err = unifying_loop(state, true, false, true);

    if(err)
    {
        // If transmission failed then a keep alive payload is still buffered.
        unifying_state_buffers_clear(state);
        return err;
    }

    err = unifying_response(state, &receive_entry, UNIFYING_PAIR_RESPONSE_1_LEN);

    if(err)
    {
        return err;
    }

    // Unpack the response.
    struct unifying_pair_response_1 pair_response_1;
    unifying_pair_response_1_unpack(&pair_response_1, receive_entry->payload);
    unifying_receive_entry_destroy(receive_entry);

    // Check that we got the correct response to our pairing request.
    if(pair_response_1.step != 1)
    {
        return UNIFYING_PAIR_STEP_ERROR;
    }

    // Check that the response was intended for us.
    if(id != pair_response_1.id)
    {
        return UNIFYING_PAIR_ID_ERROR;
    }

    // We've received a new address for all future communication with the receiver.
    // TODO: handle potential errors.
    unifying_state_address_set(state, pair_response_1.address);

    err = unifying_pair_step_2(state, crypto, serial, capabilities);

    if(err)
    {
        return err;
    }

    // Transmit the next paring request.
    err = unifying_loop(state, true, true, false);

    if(err)
    {
        // If transmission failed then a pairing request is still buffered.
        unifying_state_buffers_clear(state);
        return err;
    }

    // Send a keep alive payload so we can receive a response.
    err = unifying_loop(state, true, false, true);

    if(err)
    {
        // If transmission failed then a keep alive payload is still buffered.
        unifying_state_buffers_clear(state);
        return err;
    }

    err = unifying_response(state, &receive_entry, UNIFYING_PAIR_RESPONSE_2_LEN);

    if(err)
    {
        return err;
    }

    // Unpack the response.
    struct unifying_pair_response_2 pair_response_2;
    unifying_pair_response_2_unpack(&pair_response_2, receive_entry->payload);
    unifying_receive_entry_destroy(receive_entry);

    // Check that we got the correct response to our pairing request.
    if(pair_response_2.step != 2)
    {
        return UNIFYING_PAIR_STEP_ERROR;
    }

    err = unifying_pair_step_3(state, name, name_length);

    if(err)
    {
        return err;
    }

    // Transmit the next paring request.
    err = unifying_loop(state, true, true, false);

    if(err)
    {
        // If transmission failed then a pairing request is still buffered.
        unifying_state_buffers_clear(state);
        return err;
    }

    // Send a keep alive payload so we can receive a response.
    err = unifying_loop(state, true, false, true);

    if(err)
    {
        // If transmission failed then a keep alive payload is still buffered.
        unifying_state_buffers_clear(state);
        return err;
    }

    err = unifying_response(state, &receive_entry, UNIFYING_PAIR_RESPONSE_3_LEN);

    if(err)
    {
        return err;
    }

    // Unpack the response.
    struct unifying_pair_response_3 pair_response_3;
    unifying_pair_response_3_unpack(&pair_response_3, receive_entry->payload);
    unifying_receive_entry_destroy(receive_entry);

    // Check that we got the correct response to our pairing request.
    if(pair_response_3.step != 6)
    {
        return UNIFYING_PAIR_STEP_ERROR;
    }

    err = unifying_pair_complete(state);

    if(err)
    {
        return err;
    }

    // Transmit the next paring request.
    err = unifying_loop(state, true, true, false);

    if(err)
    {
        // If transmission failed then a pairing request is still buffered.
        unifying_state_buffers_clear(state);
        return err;
    }

    // We've received all the information that we need to create an AES key.
    // We need to deobfuscate it.
    struct unifying_proto_aes_key proto_aes_key;
    unifying_proto_aes_key_init(&proto_aes_key,
                                pair_response_1.address,
                                product_id,
                                pair_response_1.product_id,
                                crypto,
                                pair_response_2.crypto);
    uint8_t aes_buffer[UNIFYING_AES_BLOCK_LEN];
    unifying_proto_aes_key_pack(aes_buffer, &proto_aes_key);
    unifying_deobfuscate_aes_key(state->aes_key, aes_buffer);

    return UNIFYING_SUCCESS;
}

enum unifying_error unifying_set_timeout(struct unifying_state* state, uint16_t timeout)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_set_timeout_request timeout_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_SET_TIMEOUT_REQUEST_LEN, timeout);

    if(!transmit_entry)
    {
        return UNIFYING_CREATE_ERROR;
    }

    unifying_set_timeout_request_init(&timeout_request, timeout);
    unifying_set_timeout_request_pack(transmit_entry->payload, &timeout_request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
    }

    return err;
}

enum unifying_error unifying_encrypted_keystroke(struct unifying_state* state,
                                                 const uint8_t keys[UNIFYING_KEYS_LEN],
                                                 uint8_t modifiers)
{
    enum unifying_error err;
    uint8_t aes_buffer[UNIFYING_AES_DATA_LEN];
    uint8_t aes_iv[UNIFYING_AES_BLOCK_LEN];
    uint8_t payload[UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN];

    struct unifying_encrypted_keystroke_plaintext plaintext;
    struct unifying_encrypted_keystroke_iv iv;
    struct unifying_encrypted_keystroke_request request;

    unifying_encrypted_keystroke_plaintext_init(&plaintext, modifiers, keys);
    unifying_encrypted_keystroke_plaintext_pack(aes_buffer, &plaintext);

    unifying_encrypted_keystroke_iv_init(&iv, state->aes_counter);
    unifying_encrypted_keystroke_iv_pack(aes_iv, &iv);

    if(state->interface->encrypt(aes_buffer, state->aes_key, aes_iv)) {
        return UNIFYING_ENCRYPTION_ERROR;
    }

    unifying_encrypted_keystroke_request_init(&request, aes_buffer, state->aes_counter);
    unifying_encrypted_keystroke_request_pack(payload, &request);

    err = unifying_transmit(state, payload, UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN, state->default_timeout);

    if(err)
    {
        return err;
    }

    state->aes_counter++;

    if(state->interface->payload_available()) {
        return unifying_receive(state);
    }

    return UNIFYING_SUCCESS;
}
