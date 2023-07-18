
/*!
 * \file unifying_error.h
 * \brief Collection of error values, names, and messages.
 * 
 * \todo Add a preprocessor option to omit error names and messages to save space.
 * \todo make error names and messages static.
 */

#ifndef UNIFYING_ERROR_H
#define UNIFYING_ERROR_H

#include <stddef.h>

/*!
 * Error values returns by various functions.
 */
enum unifying_error {
    /// Success.
    UNIFYING_SUCCESS = 0,
    /// Generic error.
    UNIFYING_ERROR,
    /// Device name is too long.
    UNIFYING_NAME_LENGTH_ERROR,
    /// Failed to set the radio address.
    UNIFYING_SET_ADDRESS_ERROR,
    /// Failed to set the radio channel.
    UNIFYING_SET_CHANNEL_ERROR,
    /// Failed to transmit a payload.
    UNIFYING_TRANSMIT_ERROR,
    /// Failed to receive a payload.
    UNIFYING_RECEIVE_ERROR,
    /// Payload's length does not match its expected length.
    UNIFYING_PAYLOAD_LENGTH_ERROR,
    /// Payload's computed checksum does not match its stated checksum.
    UNIFYING_CHECKSUM_ERROR,
    /// Generic pairing error.
    UNIFYING_PAIR_ERROR,
    /// Received a pairing response with an unexpected step.
    UNIFYING_PAIR_STEP_ERROR,
    /// Received a pairing response with an ID that does not match the requested ID.
    UNIFYING_PAIR_ID_ERROR,
    /// Encryption failed.
    UNIFYING_ENCRYPTION_ERROR,
    /// Generic buffer error.
    UNIFYING_BUFFER_ERROR,
    /// Buffer was full when it was expected to not be full.
    UNIFYING_BUFFER_FULL_ERROR,
    /// Buffer was empty when it was expected to not be empty.
    UNIFYING_BUFFER_EMPTY_ERROR,
    /// Failed to create a dynamically allocated object.
    UNIFYING_CREATE_ERROR,
    /// The number of errors that have been defined
    UNIFYING_ERROR_COUNT,
};

/*!
 * Names of error values.
 */
extern const char* unifying_error_name[UNIFYING_ERROR_COUNT];

/*!
 * Messages associated with error values.
 */
extern const char* unifying_error_message[UNIFYING_ERROR_COUNT];

/*!
 * Get the name of the supplied error.
 * 
 * \param[in]   err Error value.
 * 
 * \return  Error name.
 */
const char* unifying_get_error_name(enum unifying_error err);

/*!
 * Get the message of the supplied error.
 * 
 * \param[in]   err Error value.
 * 
 * \return  Error message.
 */
const char* unifying_get_error_message(enum unifying_error err);

#endif
