
#include "unifying_error.h"

const char* unifying_error_name[UNIFYING_ERROR_COUNT] = {
    "SUCCESS",
    "ERROR",
    "NAME_LENGTH_ERROR",
    "SET_ADDRESS_ERROR",
    "SET_CHANNEL_ERROR",
    "TRANSMIT_ERROR",
    "RECEIVE_ERROR",
    "PAYLOAD_LENGTH_ERROR",
    "CHECKSUM_ERROR",
    "PAIR_ERROR",
    "PAIR_STEP_ERROR",
    "PAIR_ID_ERROR",
    "ENCRYPTION_ERROR",
    "BUFFER_ERROR",
    "BUFFER_FULL_ERROR",
    "BUFFER_EMPTY_ERROR",
    "CREATE_ERROR",
};

const char* unifying_error_message[UNIFYING_ERROR_COUNT] = {
    "Success",
    "Generic error",
    "Device name is too long",
    "Failed to set the radio address",
    "Failed to set the radio channel",
    "Failed to transmit a payload",
    "Failed to receive a payload",
    "Payload's length does not match its expected length",
    "Payload's computed checksum does not match its stated checksum",
    "Generic pairing error",
    "Received a pairing response with an unexpected step",
    "Received a pairing response with an ID that does not match the requested ID",
    "Encryption failed",
    "Generic buffer error",
    "Buffer was full when it was expected to not be full",
    "Buffer was empty when it was expected to not be empty",
    "Failed to create a dynamically allocated object",
};

const char* unifying_get_error_name(enum unifying_error err)
{
    return unifying_error_name[(size_t) err];
}

const char* unifying_get_error_message(enum unifying_error err)
{
    return unifying_error_message[(size_t) err];
}
