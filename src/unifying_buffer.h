
/*!
 * \file unifying_buffer.h
 * \brief Simple ring buffer used to store Unifying payloads
 */

#ifndef UNIFYING_BUFFER_H
#define UNIFYING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "unifying_error.h"

/*!
 * Ring buffer structure.
 * 
 * Stores arbitrary data pointers in a fixed length buffer
 * as well as the metadata necessary to access that data.
 */
struct unifying_ring_buffer
{
    void** buffer;
    uint8_t size;
    uint8_t count;
    uint8_t front;
    uint8_t back;
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Initialize a ring buffer instance.
 * 
 * \param[out]  ring_buffer     Pointer to a ring buffer to initialize.
 * \param[in]   buffer          Pointer to pointer buffer.
 * \param[in]   size            Size of the pointer buffer.
 * 
 * \return  \ref UNIFYING_BUFFER_ERROR if \p size is `0`.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_ring_buffer_init(struct unifying_ring_buffer* ring_buffer, void** buffer, uint8_t size);

/*!
 * Allocate and initialize a ring buffer instance.
 * 
 * Ring buffers created with this function should be freed with
 * unifying_ring_buffer_destroy() when they are no longer needed.
 * 
 * \param[in]   size    Number of pointers the allocated buffer can store.
 * 
 * \return  `NULL` if \p size is `0` or if allocation fails.
 * \return  \ref unifying_ring_buffer pointer otherwise.
 * 
 * \see     unifying_ring_buffer_destroy()
 */
struct unifying_ring_buffer* unifying_ring_buffer_create(uint8_t size);

/*!
 * Free a dynamically allocated ring buffer instance.
 * 
 * \param[in,out]   ring_buffer     Ring buffer to free.
 * 
 * \see     unifying_ring_buffer_create()
 */
void unifying_ring_buffer_destroy(struct unifying_ring_buffer* ring_buffer);

/*!
 * Add a pointer to the front of a ring buffer.
 * 
 * \param[in,out]   ring_buffer     Ring buffer to add a pointer to.
 * \param[in]       entry           Pointer to add to the ring buffer.
 * 
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_ring_buffer_push_front(struct unifying_ring_buffer* ring_buffer, void* entry);

/*!
 * Add a pointer to the back of a ring buffer.
 * 
 * \param[in,out]   ring_buffer     Ring buffer to add a pointer to.
 * \param[in]       entry           Pointer to add to the ring buffer.
 * 
 * \return  \ref UNIFYING_BUFFER_FULL_ERROR if the buffer is full.
 * \return  \ref UNIFYING_SUCCESS otherwise.
 */
enum unifying_error unifying_ring_buffer_push_back(struct unifying_ring_buffer* ring_buffer, void* entry);

/*!
 * Remove a pointer from the front of a ring buffer and return it.
 * 
 * \param[in,out]   ring_buffer     Ring buffer to remove a pointer from.
 * 
 * \return  `NULL` if the buffer is empty.
 * \return  `void*` to some data otherwise.
 */
void* unifying_ring_buffer_pop_front(struct unifying_ring_buffer* ring_buffer);

/*!
 * Remove a pointer from the back of a ring buffer and return it.
 * 
 * \param[in,out]   ring_buffer     Ring buffer to remove a pointer from.
 * 
 * \return  `NULL` if the buffer is empty.
 * \return  `void*` to some data otherwise.
 */
void* unifying_ring_buffer_pop_back(struct unifying_ring_buffer* ring_buffer);

/*!
 * Return the pointer at the front of the buffer but do not remove it from the buffer.
 * 
 * \param[in]   ring_buffer     Ring buffer to get a pointer from.
 * 
 * \return  `NULL` if the buffer is empty.
 * \return  `void*` to some data otherwise.
 */
void* unifying_ring_buffer_peek_front(struct unifying_ring_buffer* ring_buffer);

/*!
 * Return the pointer at the back of the buffer but do not remove it from the buffer.
 * 
 * \param[in]   ring_buffer     Ring buffer to get a pointer from.
 * 
 * \return  `NULL` if the buffer is empty.
 * \return  `void*` to some data otherwise.
 */
void* unifying_ring_buffer_peek_back(struct unifying_ring_buffer* ring_buffer);

/*!
 * Test if a ring buffer is empty.
 * 
 * \param[in]   ring_buffer     Ring buffer to check.
 * 
 * \return  `true` if the ring buffer is empty.
 * \return  `false` if the ring buffer is not empty.
 */
bool unifying_ring_buffer_empty(const struct unifying_ring_buffer* ring_buffer);

/*!
 * Test if a ring buffer is full.
 * 
 * \param[in]   ring_buffer     Ring buffer to check.
 * 
 * \return  `true` if the ring buffer is full.
 * \return  `false` if the ring buffer is not full.
 */
bool unifying_ring_buffer_full(const struct unifying_ring_buffer* ring_buffer);

#ifdef __cplusplus
}
#endif

#endif
