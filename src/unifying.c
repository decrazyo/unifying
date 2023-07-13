
#include "unifying.h"

enum unifying_error unifying_response(struct unifying_state* state,
                                          struct unifying_receive_entry** receive_entry,
                                          uint8_t length)
{
    *receive_entry = unifying_ring_buffer_pop_front(state->receive_buffer);

    if(!(*receive_entry))
    {
        return UNIFYING_ERROR;
    }

    if(unifying_checksum_verify((*receive_entry)->payload, (*receive_entry)->length))
    {
        unifying_receive_entry_destroy(*receive_entry);
        return UNIFYING_ERROR;
    }

    if(length && (*receive_entry)->length != length)
    {
        unifying_receive_entry_destroy(*receive_entry);
        return UNIFYING_ERROR;
    }

    return UNIFYING_SUCCESS;
}


// TODO: Refactor this
enum unifying_error unifying_hidpp_1_0(struct unifying_state* state)
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
        return UNIFYING_ERROR;
    }

    transmit_entry = unifying_transmit_entry_create(UNIFYING_HIDPP_1_0_SHORT_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        unifying_receive_entry_destroy(receive_entry);
        return UNIFYING_ERROR;
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
            return UNIFYING_BUFFER_ERROR;
        }

        uint8_t err = state->interface->transmit_payload(transmit_entry->payload, transmit_entry->length);

        if(err)
        {
            // Transmission failed.
            // Switch to a new channel.
            // TODO: Test if this works as expected.

            unifying_state_channel_set(state, unifying_next_channel(state->channel));
            return UNIFYING_TRANSMIT_ERROR;
        }

        // Transmission succeeded.
        // Adjust the timeout and determine when the next packet should be sent.
        if(transmit_entry->timeout)
        {
            state->timeout = transmit_entry->timeout;
        }
        // NOTE: It may be better to call "state->interface->time()" again here.
        state->previous_transmit = current_time;
        state->next_transmit = state->previous_transmit + state->timeout * UNIFYING_TIMEOUT_COEFFICIENT;

        // Dequeue and destroy the transmit entry since we won't need it anymore.
        unifying_transmit_entry_destroy(unifying_ring_buffer_pop_front(state->transmit_buffer));

        // Check if we have received an ACK payload.
        if(state->interface->payload_available()) {
            if(unifying_ring_buffer_full(state->receive_buffer))
            {
                // We don't have room to store the payload.
                // Maybe we will the next time that unifying_tick() is called.
                // The payload can just hang out in the radio's RX FIFO in the meantime.
                return UNIFYING_BUFFER_ERROR;
            }

            uint8_t length = state->interface->payload_size();
            struct unifying_receive_entry* receive_entry;
            receive_entry = unifying_receive_entry_create(length);

            if(!receive_entry)
            {
                return UNIFYING_ERROR;
            }

            // Buffer the received payload for now.
            // It will be handled the next time that unifying_tick() is called
            // unless another function like unifying_pair() handles it first.
            length = state->interface->receive_payload(receive_entry->payload, receive_entry->length);

            if(length != receive_entry->length)
            {
                // Somehow we received a payload of a different size than was stated earlier.
                // This should never happen.
                unifying_receive_entry_destroy(receive_entry);
                return UNIFYING_RECEIVE_ERROR;
            }

            if(unifying_ring_buffer_push_back(state->receive_buffer, receive_entry))
            {
                // The buffer didn't have enough space even though we checked it earlier.
                // This should never happen.
                unifying_receive_entry_destroy(receive_entry);
                return UNIFYING_BUFFER_ERROR;
            }
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
        return UNIFYING_STEP_ERROR;
    }

    // Check that the response was intended for us.
    if(id != pair_response_1.id)
    {
        return UNIFYING_ID_ERROR;
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
        return UNIFYING_ERROR;
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
        return UNIFYING_ERROR;
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

