
#include "unifying_data.h"

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



void unifying_long_wake_up_request_init(struct unifying_long_wake_up_request* unpacked, uint8_t index)
{
    memset(unpacked, 0, sizeof(struct unifying_long_wake_up_request));
    unpacked->index = index;
    unpacked->frame = 0x51;
    unpacked->index_2 = index;
    unpacked->unknown_4 = 0x00;
    unpacked->unknown_5_7[0] = 0x01;
    unpacked->unknown_5_7[1] = 0x01;
    unpacked->unknown_5_7[2] = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_long_wake_up_request));
}

void unifying_long_wake_up_request_pack(uint8_t packed[UNIFYING_LONG_WAKE_UP_REQUEST_LEN],
                                     const struct unifying_long_wake_up_request* unpacked)
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



void unifying_short_wake_up_request_init(struct unifying_short_wake_up_request* unpacked, uint8_t index)
{
    memset(unpacked, 0, sizeof(struct unifying_short_wake_up_request));
    unpacked->index = index;
    unpacked->frame = 0x50;
    unpacked->unknown_2 = 0x01;
    unpacked->unknown_3 = 0x4B;
    unpacked->unknown_4 = 0x01;
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_short_wake_up_request));
}

void unifying_short_wake_up_request_pack(uint8_t packed[UNIFYING_SHORT_WAKE_UP_REQUEST_LEN],
                                     const struct unifying_short_wake_up_request* unpacked)
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

void unifying_multimeia_keystroke_request_init(struct unifying_multimeia_keystroke_request* unpacked,
                                               uint8_t keys[UNIFYING_MULTIMEDIA_KEYS_LEN])
{
    memset(unpacked, 0, sizeof(struct unifying_multimeia_keystroke_request));
    unpacked->frame = 0xC3;
    memcpy(unpacked->keys, keys, sizeof(unpacked->keys));
    unpacked->checksum = unifying_checksum((uint8_t*) unpacked, sizeof(struct unifying_multimeia_keystroke_request));
}

void unifying_multimeia_keystroke_request_pack(uint8_t packed[UNIFYING_MULTIMEDIA_KEYSTROKE_REQUEST_LEN],
                                               const struct unifying_multimeia_keystroke_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    memcpy(&packed[2], unpacked->keys, sizeof(unpacked->keys));
    memcpy(&packed[6], unpacked->unknown_6_8, sizeof(unpacked->unknown_6_8));
    packed[9] = unpacked->checksum;
}

void unifying_mouse_request_init(struct unifying_mouse_request* unpacked,
                                 uint8_t buttons,
                                 int16_t move_y,
                                 int16_t move_x,
                                 int8_t wheel_y,
                                 int8_t wheel_x)
{
    memset(unpacked, 0, sizeof(struct unifying_mouse_request));
    unpacked->frame = 0xC2;
    unpacked->buttons = buttons;
    unpacked->move_x = move_x;
    unpacked->move_y = move_y;
    unpacked->wheel_x = wheel_x;
    unpacked->wheel_y = wheel_y;
}

void unifying_mouse_request_pack(uint8_t packed[UNIFYING_MOUSE_REQUEST_LEN],
                                 const struct unifying_mouse_request* unpacked)
{
    packed[0] = unpacked->unknown_0;
    packed[1] = unpacked->frame;
    packed[2] = unpacked->buttons;
    packed[3] = unpacked->unknown_3;

    // X and Y axis movement packing.
    // YY XY XX
    //  | ||  |
    //  | ||  '- bytes 4-11 of X axis movement
    //  | ||
    //  | |'- byte 8-11 of Y axis movement
    //  | |
    //  | '- bytes 0-3 of X axis movement
    //  |
    //  '- bytes 0-7 of Y axis movement
    packed[4] = unpacked->move_y & 0xFF;
    packed[5] = ((unpacked->move_y >> 8) & 0x0F) | ((unpacked->move_x << 4) & 0xF0);
    packed[6] = (unpacked->move_x >> 4) & 0xFF;

    packed[7] = unpacked->wheel_y;
    packed[8] = unpacked->wheel_x;
    packed[9] = unifying_checksum(packed, UNIFYING_MOUSE_REQUEST_LEN - 1);
}


