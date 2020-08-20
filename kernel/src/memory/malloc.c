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

    memory_chunk *chunk = NULL;

    for (size_t i = 0; i < mmap->len; i++)
    {
        if (mmap->entries[i].type != E820_ENTRY_FREE)
        {
            continue;
        }

        if (start_chunk == NULL)
        {
            /* Find start chunk. */
            if ((uint32_t)&KERNEL_END > mmap->entries[i].base &&
                (uint32_t)&KERNEL_END <= mmap->entries[i].base + mmap->entries[i].len)
            {
                start_chunk = (memory_chunk *)(uint32_t)&KERNEL_END;
                start_chunk->next_chunk = NULL;
                start_chunk->previous_chunk = NULL;
                start_chunk->allocated = false;
                start_chunk->len = mmap->entries[i].base + mmap->entries[i].len - (uint32_t)&KERNEL_END - sizeof(memory_chunk);

                chunk = start_chunk;
                len += start_chunk->len;

                info("create, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);
            }
            continue;
        }

        /* Find additional chunks. */
        memory_chunk *next_chunk = (memory_chunk *)(uint32_t)mmap->entries[i].base;
        next_chunk->next_chunk = NULL;
        next_chunk->previous_chunk = chunk;
        next_chunk->allocated = false;
        next_chunk->len = mmap->entries[i].len - sizeof(memory_chunk);

        chunk->next_chunk = next_chunk;
        chunk = next_chunk;
        len += next_chunk->len;

        info("create, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);
    }

    assert(start_chunk);

    initialized = true;
    info("initialized, length=%x", len);
}

void *malloc(const uint32_t len)
{
    assert(initialized);
    assert(len);

    /* Find a deallocated chunk that is big enough. */
    memory_chunk *chunk = NULL;
    for (chunk = start_chunk; chunk != NULL; chunk = chunk->next_chunk)
    {
        if (!chunk->allocated && chunk->len >= len)
        {
            break;
        }
    }

    assert(chunk);

    /* Check if the chunk can be split. */
    if (chunk->len > len + sizeof(memory_chunk))
    {
        memory_chunk *new_chunk = (memory_chunk *)(((uint8_t *)chunk) + len + sizeof(memory_chunk));
        new_chunk->previous_chunk = chunk;
        new_chunk->next_chunk = chunk->next_chunk;
        new_chunk->allocated = false;
        new_chunk->len = chunk->len - len - sizeof(memory_chunk);

        if (new_chunk->next_chunk != NULL)
        {
            new_chunk->next_chunk->previous_chunk = new_chunk;
        }

        /* 
         * Adjust the length of the current chunk and link with
         * the new chunk.
         */
        chunk->len = len;
        chunk->next_chunk = new_chunk;

        info("create, begin=%x, end=%x, length=%x", (uint8_t *)new_chunk + sizeof(memory_chunk), (uint8_t *)new_chunk + sizeof(memory_chunk) + new_chunk->len, new_chunk->len);
    }

    chunk->allocated = true;

    info("allocate, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);
    return (void *)((uint32_t)chunk + sizeof(memory_chunk));
}

void *realloc(void *ptr, const uint32_t len)
{
    assert(initialized);
    assert(ptr);
    assert(len);

    memory_chunk *chunk = (memory_chunk *)((uint32_t)ptr - sizeof(memory_chunk));
    info("realloc, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);

    /* Shrink chunk if needed. No new allocation is required. */
    if (len <= chunk->len)
    {
        warn("%s", "cannot shrink chunk");
        return ptr;
    }

    void *next_ptr = malloc(len);
    assert(next_ptr);

    /* Copy data from previous chunk to new chunk. */
    memcpy(next_ptr, ptr, chunk->len);

    /* Free previous chunk. */
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
    info("free, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);

    /* 
     * Check if the previous chunk can be merged with the
     * deallocated chunk. Current chunk is removed.
     */
    if (chunk->previous_chunk != NULL && !chunk->previous_chunk->allocated)
    {
        chunk->previous_chunk->next_chunk = chunk->next_chunk;
        chunk->previous_chunk->len += chunk->len + sizeof(memory_chunk);

        if (chunk->next_chunk != NULL)
        {
            chunk->next_chunk->previous_chunk = chunk->previous_chunk;
        }
        chunk = chunk->previous_chunk;
        info("merge, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);
    }

    /* 
     * Check if the next chunk can be merged with the
     * deallocated chunk. Next chunk is removed.
     */
    if (chunk->next_chunk != NULL && !chunk->next_chunk->allocated)
    {
        chunk->len += chunk->next_chunk->len + sizeof(memory_chunk);
        chunk->next_chunk = chunk->next_chunk->next_chunk;

        if (chunk->next_chunk != NULL)
        {
            chunk->next_chunk->previous_chunk = chunk;
        }
        info("merge, begin=%x, end=%x, length=%x", (uint8_t *)chunk + sizeof(memory_chunk), (uint8_t *)chunk + sizeof(memory_chunk) + chunk->len, chunk->len);
    }
}