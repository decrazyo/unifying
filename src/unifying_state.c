
#include "unifying_state.h"

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
                                                const uint8_t iv[UNIFYING_AES_BLOCK_LEN]))
{
    interface->transmit_payload = transmit_payload;
    interface->receive_payload = receive_payload;
    interface->payload_available = payload_available;
    interface->payload_size = payload_size;
    interface->set_address = set_address;
    interface->set_channel = set_channel;
    interface->time = time;
    interface->encrypt = encrypt;
}



void unifying_state_init(struct unifying_state* state,
                         const struct unifying_interface* interface,
                         struct unifying_ring_buffer* transmit_buffer,
                         struct unifying_ring_buffer* receive_buffer,
                         uint8_t address[UNIFYING_ADDRESS_LEN],
                         uint8_t aes_key[UNIFYING_AES_BLOCK_LEN],
                         uint32_t aes_counter,
                         uint16_t default_timeout,
                         uint8_t channel)
{
    state->transmit_buffer = transmit_buffer;
    state->receive_buffer = receive_buffer;
    state->interface = interface;
    state->address = address;
    state->aes_key = aes_key;
    state->aes_counter = aes_counter;
    state->default_timeout = default_timeout;
    state->timeout = default_timeout;
    state->previous_transmit = 0;
    state->next_transmit = 0;
    state->channel = channel;
}

void unifying_state_transmit_buffer_clear(struct unifying_state* state)
{
    while(!unifying_ring_buffer_empty(state->transmit_buffer))
    {
        unifying_transmit_entry_destroy(unifying_ring_buffer_pop_front(state->transmit_buffer));
    }
}

void unifying_state_receive_buffer_clear(struct unifying_state* state)
{
    while(!unifying_ring_buffer_empty(state->receive_buffer))
    {
        unifying_receive_entry_destroy(unifying_ring_buffer_pop_front(state->receive_buffer));
    }
}

void unifying_state_buffers_clear(struct unifying_state* state)
{
    unifying_state_transmit_buffer_clear(state);
    unifying_state_receive_buffer_clear(state);
}

void unifying_state_channel_set(struct unifying_state* state, uint8_t channel)
{
    state->channel = channel;
    state->interface->set_channel(state->channel);
}

void unifying_state_address_set(struct unifying_state* state, const uint8_t address[UNIFYING_ADDRESS_LEN])
{
    memcpy(state->address, address, UNIFYING_ADDRESS_LEN);
    state->interface->set_address(state->address);
}

void unifying_transmit_entry_init(struct unifying_transmit_entry* entry,
                                  uint8_t* payload,
                                  uint8_t length,
                                  uint8_t timeout)
{
    entry->payload = payload;
    entry->length = length;
    entry->timeout = timeout;
}

struct unifying_transmit_entry* unifying_transmit_entry_create(uint8_t length, uint8_t timeout)
{
    struct unifying_transmit_entry* entry = malloc(sizeof(struct unifying_transmit_entry));

    if(!entry)
    {
        return NULL;
    }

    uint8_t* payload = malloc(length);

    if(!payload)
    {
        free(entry);
        return NULL;
    }

    unifying_transmit_entry_init(entry, payload, length, timeout);
    return entry;
}

void unifying_transmit_entry_destroy(struct unifying_transmit_entry* entry)
{
    free(entry->payload);
    free(entry);
}



void unifying_receive_entry_init(struct unifying_receive_entry* entry,
                                         uint8_t* payload,
                                         uint8_t length)
{
    entry->payload = payload;
    entry->length = length;
}

struct unifying_receive_entry* unifying_receive_entry_create(uint8_t length)
{
    struct unifying_receive_entry* entry = malloc(sizeof(struct unifying_receive_entry));

    if(!entry)
    {
        return NULL;
    }

    uint8_t* payload = malloc(length);

    if(!payload)
    {
        free(entry);
        return NULL;
    }

    unifying_receive_entry_init(entry, payload, length);
    return entry;
}

void unifying_receive_entry_destroy(struct unifying_receive_entry* entry)
{
    free(entry->payload);
    free(entry);
}
