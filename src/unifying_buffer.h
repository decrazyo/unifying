
#ifndef UNIFYING_BUFFER_H
#define UNIFYING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "unifying_error.h"

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

enum unifying_error unifying_ring_buffer_init(struct unifying_ring_buffer* ring_buffer, void** buffer, uint8_t size);
struct unifying_ring_buffer* unifying_ring_buffer_create(uint8_t size);
void unifying_ring_buffer_destroy(struct unifying_ring_buffer* ring_buffer);
enum unifying_error unifying_ring_buffer_push_front(struct unifying_ring_buffer* ring_buffer, void* entry);
enum unifying_error unifying_ring_buffer_push_back(struct unifying_ring_buffer* ring_buffer, void* entry);
void* unifying_ring_buffer_pop_front(struct unifying_ring_buffer* ring_buffer);
void* unifying_ring_buffer_pop_back(struct unifying_ring_buffer* ring_buffer);
void* unifying_ring_buffer_peek_front(struct unifying_ring_buffer* ring_buffer);
void* unifying_ring_buffer_peek_back(struct unifying_ring_buffer* ring_buffer);
bool unifying_ring_buffer_empty(const struct unifying_ring_buffer* ring_buffer);
bool unifying_ring_buffer_full(const struct unifying_ring_buffer* ring_buffer);

#ifdef __cplusplus
}
#endif

#endif
