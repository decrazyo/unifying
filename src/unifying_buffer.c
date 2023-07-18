
#include "unifying_buffer.h"

enum unifying_error unifying_ring_buffer_init(struct unifying_ring_buffer* ring_buffer, void** buffer, uint8_t size)
{
    if(!size)
    {
        return UNIFYING_BUFFER_ERROR;
    }

    ring_buffer->buffer = buffer;
    ring_buffer->size = size;
    ring_buffer->count = 0;
    ring_buffer->front = 0;
    ring_buffer->back = size - 1;
    return UNIFYING_SUCCESS;
}

struct unifying_ring_buffer* unifying_ring_buffer_create(uint8_t size)
{
    if(!size)
    {
        return NULL;
    }

    struct unifying_ring_buffer* ring_buffer = malloc(sizeof(struct unifying_ring_buffer));

    if(!ring_buffer)
    {
        return NULL;
    }

    void** buffer = malloc(size * sizeof(void*));

    if(!buffer)
    {
        free(ring_buffer);
        return NULL;
    }

    unifying_ring_buffer_init(ring_buffer, buffer, size);
    return ring_buffer;
}

void unifying_ring_buffer_destroy(struct unifying_ring_buffer* ring_buffer)
{
    free(ring_buffer->buffer);
    free(ring_buffer);
}

enum unifying_error unifying_ring_buffer_push_front(struct unifying_ring_buffer* ring_buffer, void* entry)
{
    if(unifying_ring_buffer_full(ring_buffer))
    {
        return UNIFYING_BUFFER_FULL_ERROR;
    }

    if(ring_buffer->front)
    {
        ring_buffer->front -= 1;
    }
    else
    {
        ring_buffer->front = ring_buffer->size - 1;
    }

    ring_buffer->buffer[ring_buffer->front] = entry;
    ring_buffer->count += 1;
    return UNIFYING_SUCCESS;
}

enum unifying_error unifying_ring_buffer_push_back(struct unifying_ring_buffer* ring_buffer, void* entry)
{
    if(unifying_ring_buffer_full(ring_buffer))
    {
        return UNIFYING_BUFFER_FULL_ERROR;
    }

    if(ring_buffer->back >= ring_buffer->size - 1)
    {
        ring_buffer->back = 0;
    }
    else {
        ring_buffer->back += 1;
    }

    ring_buffer->buffer[ring_buffer->back] = entry;
    ring_buffer->count += 1;
    return UNIFYING_SUCCESS;
}

void* unifying_ring_buffer_pop_front(struct unifying_ring_buffer* ring_buffer)
{
    if(unifying_ring_buffer_empty(ring_buffer))
    {
        return NULL;
    }

    void* entry = ring_buffer->buffer[ring_buffer->front];
    ring_buffer->count -= 1;

    if(ring_buffer->front >= ring_buffer->size - 1)
    {
        ring_buffer->front = 0;
    }
    else {
        ring_buffer->front += 1;
    }

    return entry;
}

void* unifying_ring_buffer_pop_back(struct unifying_ring_buffer* ring_buffer)
{
    if(unifying_ring_buffer_empty(ring_buffer))
    {
        return NULL;
    }

    void* entry = ring_buffer->buffer[ring_buffer->back];
    ring_buffer->count -= 1;

    if(ring_buffer->back)
    {
        ring_buffer->back -= 1;
    }
    else
    {
        ring_buffer->back = ring_buffer->size - 1;
    }

    return entry;
}

void* unifying_ring_buffer_peek_front(struct unifying_ring_buffer* ring_buffer)
{
    if(unifying_ring_buffer_empty(ring_buffer))
    {
        return NULL;
    }

    return ring_buffer->buffer[ring_buffer->front];
}

void* unifying_ring_buffer_peek_back(struct unifying_ring_buffer* ring_buffer)
{
    if(unifying_ring_buffer_empty(ring_buffer))
    {
        return NULL;
    }

    return ring_buffer->buffer[ring_buffer->back];
}

bool unifying_ring_buffer_empty(const struct unifying_ring_buffer* ring_buffer)
{
    return !ring_buffer->count;
}

bool unifying_ring_buffer_full(const struct unifying_ring_buffer* ring_buffer)
{
    return ring_buffer->count >= ring_buffer->size;
}
