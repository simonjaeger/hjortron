#include "memory/malloc.h"
#include "debug.h"

typedef struct memory_chunk
{
    struct memory_chunk *previous_chunk;
    struct memory_chunk *next_chunk;
    bool allocated;
    uint32_t size;
} memory_chunk;

static bool initialized = false;
static memory_chunk *start_chunk;

void malloc_init(uint32_t heap_begin, uint32_t heap_size)
{
    // TODO: Check size.
    // TODO: Zero out heap.

    start_chunk = (memory_chunk *)heap_begin;
    start_chunk->next_chunk = NULL;
    start_chunk->previous_chunk = NULL;
    start_chunk->allocated = false;
    start_chunk->size = heap_size;

    initialized = true;

    debug("begin=%x size=%x", heap_begin, heap_size);
}

void *malloc(uint32_t size)
{
    if (!initialized)
    {
        return NULL;
    }

    // Find an unallocated chunk that is big enough.
    memory_chunk *current_chunk = NULL;
    for (current_chunk = start_chunk;
         current_chunk != NULL;
         current_chunk = current_chunk->next_chunk)
    {
        if (!current_chunk->allocated && current_chunk->size >= size)
        {
            break;
        }
    }

    if (current_chunk == NULL)
    {
        return NULL;
    }

    // Check if the chunk can be split.
    if (current_chunk->size > size + sizeof(memory_chunk))
    {
        memory_chunk *new_chunk = (memory_chunk *)((uint32_t)current_chunk + size + sizeof(memory_chunk));
        new_chunk->previous_chunk = current_chunk;
        new_chunk->next_chunk = current_chunk->next_chunk;
        new_chunk->allocated = false;
        new_chunk->size = current_chunk->size - size - sizeof(memory_chunk);

        // Adjust the size of the current chunk and link with
        // the new chunk.
        current_chunk->size = size;
        current_chunk->next_chunk = new_chunk;
    }

    current_chunk->allocated = true;
    debug("allocated %d bytes", size);

    return (void *)((uint32_t)current_chunk + sizeof(memory_chunk));
}

void free(void *ptr)
{
    if (!initialized)
    {
        return;
    }

    memory_chunk *chunk = (memory_chunk *)((uint32_t)ptr - sizeof(memory_chunk));

    if (!chunk->allocated)
    {
        debug("%s", "cannot deallocate chunk");
        return;
    }

    chunk->allocated = false;
    debug("freed %d bytes", chunk->size);

    // Check if the previous chunk can be merged with the
    // deallocated chunk. Current chunk is removed.
    if (chunk->previous_chunk != NULL && !chunk->previous_chunk->allocated)
    {
        chunk->previous_chunk->next_chunk = chunk->next_chunk;
        chunk->previous_chunk->size += chunk->size + sizeof(memory_chunk);

        if (chunk->next_chunk != NULL)
        {
            chunk->next_chunk->previous_chunk = chunk->previous_chunk;
        }

        chunk = chunk->previous_chunk;
        debug("merged chunks to %d bytes", chunk->size);
    }

    // Check if the next chunk can be merged with the
    // deallocated chunk. Next chunk is removed.
    if (chunk->next_chunk != NULL && !chunk->next_chunk->allocated)
    {
        chunk->size += chunk->next_chunk->size + sizeof(memory_chunk);
        chunk->next_chunk = chunk->next_chunk->next_chunk;

        if (chunk->next_chunk != NULL)
        {
            chunk->previous_chunk = chunk;
        }
        debug("merged chunks to %d bytes", chunk->size);
    }
}