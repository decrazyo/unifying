
#include "unifying_data.h"

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



void unifying_pair_request_1_init(struct unifying_pair_request_1* unpacked,
                                  uint8_t id,
                                  uint16_t timeout,
                                  uint16_t product_id,
                                  uint16_t device_type)
{
    memset(unpacked, 0, sizeof(struct unifying_pair_request_1));
    unpacked->id = id;
    unpacked->frame = 0x5F;
    unpacked->step = 0x01;
    unpacked->timeout = timeout; // 8 for mice, 20 for keyboards.
    unpacked->product_id = product_id;
    unpacked->protocol = 0x04; // Unifying protocol.
    unpacked->device_type = device_type;
    unpacked->unknown_20 = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_pair_request_1));
}

void unifying_pair_request_1_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_1_LEN],
                                  const struct unifying_pair_request_1* unpacked)
{
    packed[0] = unpacked->id;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->step;
    memcpy(&packed[3], unpacked->unknown_3_7, sizeof(unpacked->unknown_3_7));
    packed[8] = unpacked->timeout;
    unifying_uint16_pack(&packed[9], unpacked->product_id);
    packed[11] = unpacked->protocol;
    packed[12] = unpacked->unknown_12;
    unifying_uint16_pack(&packed[13], unpacked->device_type);
    memcpy(&packed[15], unpacked->unknown_15_19, sizeof(unpacked->unknown_15_19));
    packed[20] = unpacked->unknown_20;
    packed[21] = unpacked->checksum;
}

void unifying_pair_response_1_unpack(struct unifying_pair_response_1* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_1_LEN])
{
    unpacked->id = packed[0];
    unpacked->frame = packed[1];
    unpacked->step = packed[2];
    memcpy(unpacked->address, &packed[3], sizeof(unpacked->address));
    unpacked->unknown_8 = packed[8];
    unifying_uint16_unpack(&unpacked->product_id, &packed[9]);
    memcpy(unpacked->unknown_11_12, &packed[11], sizeof(unpacked->unknown_11_12));
    unifying_uint16_unpack(&unpacked->device_type, &packed[13]);
    memcpy(unpacked->unknown_15_20, &packed[15], sizeof(unpacked->unknown_15_20));
    unpacked->checksum = packed[21];
}



void unifying_pair_request_2_init(struct unifying_pair_request_2* unpacked,
                                  uint32_t crypto,
                                  uint32_t serial,
                                  uint16_t capabilities)
{
    memset(unpacked, 0, sizeof(struct unifying_pair_request_2));
    unpacked->frame = 0x5F;
    unpacked->step = 0x02;
    unpacked->crypto = crypto;
    unpacked->serial = serial;
    unpacked->capabilities = capabilities;
    unpacked->unknown_13_20[15] = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_pair_request_2));
}

void unifying_pair_request_2_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_2_LEN],
                                  const struct unifying_pair_request_2* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->step;
    unifying_uint32_pack(&packed[3], unpacked->crypto);
    unifying_uint32_pack(&packed[7], unpacked->serial);
    unifying_uint16_pack(&packed[11], unpacked->capabilities);
    memcpy(&packed[13], unpacked->unknown_13_20, sizeof(unpacked->unknown_13_20));
    packed[21] = unpacked->checksum;
}

void unifying_pair_response_2_unpack(struct unifying_pair_response_2* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_2_LEN])
{
    unpacked->unknown_0 = packed[0];
    unpacked->frame = packed[1];
    unpacked->step = packed[2];
    unifying_uint32_unpack(&unpacked->crypto, &packed[3]);
    unifying_uint32_unpack(&unpacked->serial, &packed[7]);
    unifying_uint16_unpack(&unpacked->capabilities, &packed[11]);
    memcpy(unpacked->unknown_13_20, &packed[13], sizeof(unpacked->unknown_13_20));
    unpacked->checksum = packed[21];
}



void unifying_pair_request_3_init(struct unifying_pair_request_3* unpacked, const char* name, uint8_t name_length)
{
    memset(unpacked, 0, sizeof(struct unifying_pair_request_3));
    unpacked->frame = 0x5F;
    unpacked->step = 0x03;
    unpacked->unknown_3 = 0x01;
    unpacked->name_length = name_length;
    memcpy(unpacked->name, name, name_length);
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_pair_request_3));
}

void unifying_pair_request_3_pack(uint8_t packed[UNIFYING_PAIR_REQUEST_3_LEN],
                                  const struct unifying_pair_request_3* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->step;
    packed[3] = unpacked->unknown_3;
    packed[4] = unpacked->name_length;
    memcpy(&packed[5], unpacked->name, sizeof(unpacked->name));
    packed[21] = unpacked->checksum;
}

void unifying_pair_response_3_unpack(struct unifying_pair_response_3* unpacked,
                                     const uint8_t packed[UNIFYING_PAIR_RESPONSE_3_LEN])
{
    unpacked->unknown_0 = packed[0];
    unpacked->frame = packed[1];
    unpacked->step = packed[2];
    memcpy(unpacked->unknown_3_8, &packed[3], sizeof(unpacked->unknown_3_8));
    unpacked->checksum = packed[9];
}

void unifying_pair_complete_request_init(struct unifying_pair_complete_request* unpacked)
{
    memset(unpacked, 0, sizeof(struct unifying_pair_complete_request));
    unpacked->frame = 0x0F;
    unpacked->step = 0x06;
    unpacked->unknown_3 = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_pair_complete_request));
}

void unifying_pair_complete_request_pack(uint8_t packed[UNIFYING_PAIR_COMPLETE_REQUEST_LEN],
                                         const struct unifying_pair_complete_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->step;
    packed[3] = unpacked->unknown_3;
    memcpy(&packed[4], unpacked->unknown_4_8, sizeof(unpacked->unknown_4_8));
    packed[9] = unpacked->checksum;
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



void unifying_wake_up_request_1_init(struct unifying_wake_up_request_1* unpacked, uint8_t index)
{
    memset(unpacked, 0, sizeof(struct unifying_wake_up_request_1));
    unpacked->index = index;
    unpacked->frame = 0x51;
    unpacked->index_2 = index;
    unpacked->unknown_4 = 0x00;
    unpacked->unknown_5_7[0] = 0x01;
    unpacked->unknown_5_7[1] = 0x01;
    unpacked->unknown_5_7[2] = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_wake_up_request_1));
}

void unifying_wake_up_request_1_pack(uint8_t packed[UNIFYING_WAKE_UP_REQUEST_1_LEN],
                                     const struct unifying_wake_up_request_1* unpacked)
{
    packed[0] = unpacked->index;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->index_2;
    packed[3] = unpacked->unknown_3;
    packed[4] = unpacked->unknown_4;
    memcpy(&packed[5], unpacked->unknown_5_7, sizeof(unpacked->unknown_5_7));
    memcpy(&packed[8], unpacked->unknown_8_20, sizeof(unpacked->unknown_8_20));
    packed[21] = unpacked->checksum;
}



void unifying_wake_up_request_2_init(struct unifying_wake_up_request_2* unpacked, uint8_t index)
{
    memset(unpacked, 0, sizeof(struct unifying_wake_up_request_2));
    unpacked->index = index;
    unpacked->frame = 0x50;
    unpacked->unknown_2 = 0x01;
    unpacked->unknown_3 = 0x4B;
    unpacked->unknown_4 = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_wake_up_request_2));
}

void unifying_wake_up_request_2_pack(uint8_t packed[UNIFYING_WAKE_UP_REQUEST_2_LEN],
                                     const struct unifying_wake_up_request_2* unpacked)
{
    packed[0] = unpacked->index;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->unknown_2;
    packed[3] = unpacked->unknown_3;
    packed[4] = unpacked->unknown_4;
    memcpy(&packed[5], unpacked->unknown_5_8, sizeof(unpacked->unknown_5_8));
    packed[9] = unpacked->checksum;
}



void unifying_set_timeout_request_init(struct unifying_set_timeout_request* unpacked, uint16_t timeout)
{
    memset(unpacked, 0, sizeof(struct unifying_set_timeout_request));
    unpacked->frame = 0x4F;
    unpacked->timeout = timeout;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_set_timeout_request));
}

void unifying_set_timeout_request_pack(uint8_t packed[UNIFYING_SET_TIMEOUT_REQUEST_LEN],
                                       const struct unifying_set_timeout_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->unknown_2;
    unifying_uint16_pack(&packed[3], unpacked->timeout);
    memcpy(&packed[5], unpacked->unknown_5_8, sizeof(unpacked->unknown_5_8));
    packed[9] = unpacked->checksum;
}



void unifying_keep_alive_request_init(struct unifying_keep_alive_request* unpacked, uint16_t timeout)
{
    memset(unpacked, 0, sizeof(struct unifying_keep_alive_request));
    unpacked->frame = 0x40;
    unpacked->timeout = timeout;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_keep_alive_request));
}

void unifying_keep_alive_request_pack(uint8_t packed[UNIFYING_KEEP_ALIVE_REQUEST_LEN],
                                     const struct unifying_keep_alive_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    unifying_uint16_pack(&packed[2], unpacked->timeout);
    packed[4] = unpacked->checksum;
}



void unifying_hidpp_1_0_short_init(struct unifying_hidpp_1_0_short* unpacked,
                                  uint8_t index,
                                  uint8_t sub_id,
                                  uint8_t params[UNIFYING_HIDPP_1_0_SHORT_PARAMS_LEN])
{
    memset(unpacked, 0, sizeof(struct unifying_hidpp_1_0_short));
    unpacked->report = 0x10;
    unpacked->index = index;
    unpacked->sub_id = sub_id;
    memcpy(unpacked->params, params, sizeof(unpacked->params));
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_hidpp_1_0_short));
}

void unifying_hidpp_1_0_short_pack(uint8_t packed[UNIFYING_HIDPP_1_0_SHORT_LEN],
                                  const struct unifying_hidpp_1_0_short* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->report;
    packed[2] = unpacked->index;
    packed[3] = unpacked->sub_id;
    memcpy(&packed[4], unpacked->params, sizeof(unpacked->params));
    packed[8] = unpacked->unknown_8;
    packed[9] = unpacked->checksum;
}

void unifying_hidpp_1_0_short_unpack(struct unifying_hidpp_1_0_short* unpacked,
                                     const uint8_t packed[UNIFYING_HIDPP_1_0_SHORT_LEN])
{
    unpacked->unknown_0 = packed[0];
    unpacked->report = packed[1];
    unpacked->index = packed[2];
    unpacked->sub_id = packed[3];
    memcpy(unpacked->params, &packed[4], sizeof(unpacked->params));
    unpacked->unknown_8 = packed[8];
    unpacked->checksum = packed[9];
}



void unifying_hidpp_1_0_long_init(struct unifying_hidpp_1_0_long* unpacked,
                                 uint8_t index,
                                 uint8_t sub_id,
                                 uint8_t params[UNIFYING_HIDPP_1_0_LONG_PARAMS_LEN])
{
    memset(unpacked, 0, sizeof(struct unifying_hidpp_1_0_long));
    unpacked->report = 0x11;
    unpacked->index = index;
    unpacked->sub_id = sub_id;
    memcpy(unpacked->params, params, sizeof(unpacked->params));
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_hidpp_1_0_long));
}

void unifying_hidpp_1_0_long_pack(uint8_t packed[UNIFYING_HIDPP_1_0_LONG_LEN],
                                 const struct unifying_hidpp_1_0_long* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->report;
    packed[2] = unpacked->index;
    packed[3] = unpacked->sub_id;
    memcpy(&packed[4], unpacked->params, sizeof(unpacked->params));
    packed[9] = unpacked->checksum;
}

void unifying_hidpp_1_0_long_unpack(struct unifying_hidpp_1_0_long* unpacked,
                                     const uint8_t packed[UNIFYING_HIDPP_1_0_LONG_LEN])
{
    unpacked->unknown_0 = packed[0];
    unpacked->report = packed[1];
    unpacked->index = packed[2];
    unpacked->sub_id = packed[3];
    memcpy(unpacked->params, &packed[4], sizeof(unpacked->params));
    unpacked->checksum = packed[9];
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



void unifying_encrypted_keystroke_request_init(struct unifying_encrypted_keystroke_request* unpacked,
                                               uint8_t ciphertext[UNIFYING_AES_DATA_LEN],
                                               uint32_t counter)
{
    memset(unpacked, 0, sizeof(struct unifying_encrypted_keystroke_request));
    unpacked->frame = 0xD3;
    memcpy(unpacked->ciphertext, ciphertext, sizeof(unpacked->ciphertext));
    unpacked->counter = counter;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_encrypted_keystroke_request));
}

void unifying_encrypted_keystroke_request_pack(uint8_t packed[UNIFYING_ENCRYPTED_KEYSTROKE_REQUEST_LEN],
                                               const struct unifying_encrypted_keystroke_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    memcpy(&packed[2], unpacked->ciphertext, sizeof(unpacked->ciphertext));
    unifying_uint32_pack(&packed[10], unpacked->counter);
    memcpy(&packed[14], unpacked->unknown_14_20, sizeof(unpacked->unknown_14_20));
    packed[21] = unpacked->checksum;
}



// TODO: Define other keystroke types here.



void unifying_mouse_move_request_init(struct unifying_mouse_move_request* unpacked,
                                      uint8_t buttons,
                                      int16_t move_x,
                                      int16_t move_y,
                                      int8_t wheel_x,
                                      int8_t wheel_y)
{
    memset(unpacked, 0, sizeof(struct unifying_mouse_move_request));
    unpacked->frame = 0xC2;
    unpacked->buttons = buttons;
    unpacked->move_x = move_x;
    unpacked->move_y = move_y;
    unpacked->wheel_x = wheel_x;
    unpacked->wheel_y = wheel_y;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_mouse_move_request));
}

void unifying_mouse_move_request_pack(uint8_t packed[UNIFYING_MOUSE_MOVE_REQUEST_LEN],
                                      const struct unifying_mouse_move_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->buttons;
    packed[3] = unpacked->unknown_3;

    // X and Y movement is packed as a pair of big-endian signed 12-bit integers.
    // The X and Y movement data is expected to have already been clamped to a signed 12-bit range
    // with unifying_clamp_int12() prior to calling this function.
    packed[4] = (unpacked->move_x >> 4) & 0xFF;
    packed[5] = ((unpacked->move_x << 4) | (unpacked->move_y >> 8)) & 0xFF;
    packed[6] = (unpacked->move_y >> 0) & 0xFF;

    packed[7] = unpacked->wheel_x;
    packed[8] = unpacked->wheel_y;
    packed[9] = unpacked->checksum;
}


