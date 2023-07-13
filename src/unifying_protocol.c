
#include "unifying_protocol.h"

enum unifying_error unifying_pair_step_1(struct unifying_state* state,
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
        return UNIFYING_ERROR;
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

enum unifying_error unifying_pair_step_2(struct unifying_state* state,
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
        return UNIFYING_ERROR;
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

enum unifying_error unifying_pair_step_3(struct unifying_state* state,
                                                    const char* name,
                                                    uint8_t name_length)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_pair_request_3 pair_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_PAIR_REQUEST_3_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_ERROR;
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

enum unifying_error unifying_pair_complete(struct unifying_state* state)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_pair_complete_request pair_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_PAIR_COMPLETE_REQUEST_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_ERROR;
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


enum unifying_error unifying_set_timeout(struct unifying_state* state, uint16_t timeout)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_set_timeout_request timeout_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_SET_TIMEOUT_REQUEST_LEN, timeout);

    if(!transmit_entry)
    {
        return UNIFYING_ERROR;
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

enum unifying_error unifying_keep_alive(struct unifying_state* state, uint16_t timeout)
{
    enum unifying_error err;
    struct unifying_transmit_entry* transmit_entry;
    struct unifying_keep_alive_request keep_alive_request;

    transmit_entry = unifying_transmit_entry_create(UNIFYING_KEEP_ALIVE_REQUEST_LEN, 0);

    if(!transmit_entry)
    {
        return UNIFYING_ERROR;
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



enum unifying_error unifying_encrypted_keystroke(struct unifying_state* state,
                                                 const uint8_t keys[UNIFYING_KEYS_LEN],
                                                 uint8_t modifiers)
{
    enum unifying_error err;
    uint8_t aes_buffer[UNIFYING_AES_DATA_LEN];
    uint8_t aes_iv[UNIFYING_AES_BLOCK_LEN];

    struct unifying_transmit_entry* transmit_entry;
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


    transmit_entry = unifying_transmit_entry_create(UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN, state->default_timeout);

    if(!transmit_entry)
    {
        return UNIFYING_ERROR;
    }

    unifying_encrypted_keystroke_request_init(&request, aes_buffer, state->aes_counter);
    unifying_encrypted_keystroke_request_pack(transmit_entry->payload, &request);

    err = unifying_ring_buffer_push_back(state->transmit_buffer, transmit_entry);

    if(err)
    {
        unifying_transmit_entry_destroy(transmit_entry);
        return err;
    }

    state->aes_counter++;

    return UNIFYING_SUCCESS;
}
