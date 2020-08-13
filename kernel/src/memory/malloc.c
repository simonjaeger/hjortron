#include <stdbool.h>
#include "memory/malloc.h"
#include "debug.h"
#include "assert.h"

typedef struct memory_chunk
{
    struct memory_chunk *previous_chunk;
    struct memory_chunk *next_chunk;
    bool allocated;
    uint32_t len;
} memory_chunk;

extern void *KERNEL_START;
extern void *KERNEL_END;

static bool initialized = false;
static memory_chunk *start_chunk = NULL;
static size_t len;

void malloc_init(const memory_map *mmap)
{
    assert(mmap);

    memory_chunk *current_chunk = NULL;

    for (size_t i = 0; i < mmap->len; i++)
    {
        if (mmap->entries[i].type != E820_ENTRY_FREE)
        {
            continue;
        }

        if (start_chunk == NULL)
        {
            // Find start chunk.
            if ((uint32_t)&KERNEL_END > mmap->entries[i].base &&
                (uint32_t)&KERNEL_END <= mmap->entries[i].base + mmap->entries[i].len)
            {
                start_chunk = (memory_chunk *)(uint32_t)&KERNEL_END;
                start_chunk->next_chunk = NULL;
                start_chunk->previous_chunk = NULL;
                start_chunk->allocated = false;
                start_chunk->len = mmap->entries[i].base + mmap->entries[i].len - (uint32_t)&KERNEL_END - sizeof(memory_chunk);

                current_chunk = start_chunk;
                len += start_chunk->len;

                info("found chunk, base=%x length=%x", (uint32_t)current_chunk, current_chunk->len);
            }
            continue;
        }

        // Find additional chunks.
        memory_chunk *next_chunk = (memory_chunk *)(uint32_t)mmap->entries[i].base;
        next_chunk->next_chunk = NULL;
        next_chunk->previous_chunk = current_chunk;
        next_chunk->allocated = false;
        next_chunk->len = mmap->entries[i].len - sizeof(memory_chunk);

        current_chunk->next_chunk = next_chunk;
        current_chunk = next_chunk;
        len += next_chunk->len;

        info("found chunk, base=%x length=%x", (uint32_t)current_chunk, current_chunk->len);
    }

    assert(start_chunk);

    initialized = true;
    info("initialized, length=%x", len);
}

size_t malloc_deallocated()
{
    size_t len = 0;
    for (memory_chunk *current_chunk = start_chunk;
         current_chunk != NULL;
         current_chunk = current_chunk->next_chunk)
    {
        if (!current_chunk->allocated)
        {
            len += current_chunk->len + sizeof(memory_chunk);
        }
    }
    return len;
}

void *malloc(const uint32_t len)
{
    assert(initialized);
    assert(len);

    // Find an deallocated chunk that is big enough.
    memory_chunk *current_chunk = NULL;
    for (current_chunk = start_chunk;
         current_chunk != NULL;
         current_chunk = current_chunk->next_chunk)
    {
        if (!current_chunk->allocated && current_chunk->len >= len)
        {
            break;
        }
    }

    assert(current_chunk);

    // Check if the chunk can be split.
    if (current_chunk->len > len + sizeof(memory_chunk))
    {
        memory_chunk *new_chunk = (memory_chunk *)(((uint8_t *)current_chunk) + len + sizeof(memory_chunk));
        new_chunk->previous_chunk = current_chunk;
        new_chunk->next_chunk = current_chunk->next_chunk;
        new_chunk->allocated = false;
        new_chunk->len = current_chunk->len - len - sizeof(memory_chunk);

        // Adjust the length  of the current chunk and link with
        // the new chunk.
        current_chunk->len = len;
        current_chunk->next_chunk = new_chunk;
    }

    current_chunk->allocated = true;
    info("allocate, length=%x/%x, current=%x next=%x", len, malloc_deallocated(), ((uint8_t *)current_chunk) + sizeof(memory_chunk), ((uint8_t *)current_chunk->next_chunk) + sizeof(memory_chunk));

    return (void *)((uint32_t)current_chunk + sizeof(memory_chunk));
}

void *realloc(void *ptr, const uint32_t len)
{
    assert(initialized);
    assert(ptr);
    assert(len);

    memory_chunk *chunk = (memory_chunk *)((uint32_t)ptr - sizeof(memory_chunk));

    // Shrink chunk if needed. No new allocation is required.
    if (len <= chunk->len)
    {
        warn("%s", "cannot shrink chunk");
        return ptr;
    }

    void *next_ptr = malloc(len);
    assert(next_ptr);

    // Copy data from previous chunk to new chunk.
    memcpy(next_ptr, ptr, chunk->len);

    // Free previous chunk.
    free(ptr);
    return next_ptr;
}

void free(void *ptr)
{
    assert(initialized);
    assert(ptr);

    memory_chunk *chunk = (memory_chunk *)((uint32_t)ptr - sizeof(memory_chunk));
    assert(chunk->allocated);

    chunk->allocated = false;
    info("free, length=%x/%x, current=%x", chunk->len, malloc_deallocated(), ptr);

    // Check if the previous chunk can be merged with the
    // deallocated chunk. Current chunk is removed.
    if (chunk->previous_chunk != NULL && !chunk->previous_chunk->allocated)
    {
        chunk->previous_chunk->next_chunk = chunk->next_chunk;
        chunk->previous_chunk->len += chunk->len + sizeof(memory_chunk);

        if (chunk->next_chunk != NULL)
        {
            chunk->next_chunk->previous_chunk = chunk->previous_chunk;
        }
        chunk = chunk->previous_chunk;
        info("merge, mode=previous, length=%x/%x", chunk->len, malloc_deallocated());
    }

    // Check if the next chunk can be merged with the
    // deallocated chunk. Next chunk is removed.
    if (chunk->next_chunk != NULL && !chunk->next_chunk->allocated)
    {
        chunk->len += chunk->next_chunk->len + sizeof(memory_chunk);
        chunk->next_chunk = chunk->next_chunk->next_chunk;

        if (chunk->next_chunk != NULL)
        {
            chunk->next_chunk->previous_chunk = chunk;
        }
        info("merge, mode=next, length=%x/%x", chunk->len, malloc_deallocated());
    }
}