#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "umalloc.h"

/* Warning: the guard string must be 8-byte length*/
#define HEAD_GUARD  "DEADBEEF"
#define TAIL_GUARD  "DEADBEEF"

#define HEAD_OVERHEAD_LEN   (sizeof(size_t) + 8)
#define TAIL_OVERHEAD_LEN   (8)
#define TOTAL_OVERHEAD_LEN  (HEAD_OVERHEAD_LEN + TAIL_OVERHEAD_LEN)

#define MAX_CHUNK_SIZE_LEN  (SIZE_MAX - TOTAL_OVERHEAD_LEN)

static void get_chunk_attrs(
                void *ptr,
                size_t **pp_chunk_size,
                char **pp_head_guard,
                char **pp_tail_guard);

static void get_chunk_attrs(
                void *ptr,
                size_t **pp_chunk_size,
                char **pp_head_guard,
                char **pp_tail_guard)
{
    size_t chunk_size = 0;

    *pp_chunk_size = (size_t*)(((char*)ptr) - HEAD_OVERHEAD_LEN);
    chunk_size = **pp_chunk_size;
    *pp_head_guard = (char*)((*pp_chunk_size) + 1);
    *pp_tail_guard = ((char*)ptr) + chunk_size;

    /* Check up out-of-bound memory error */
    assert(!memcmp(*pp_head_guard, HEAD_GUARD, 8));

    /* Check down out-of-bound memory error */
    assert(!memcmp(*pp_tail_guard, TAIL_GUARD, 8));

    return;
}

void *umalloc(size_t size)
{
    char *p = NULL;
    char *p_chunk = NULL;

    if (!size)
    {
        return NULL;
    }

    if (size > MAX_CHUNK_SIZE_LEN)
    {
        return NULL;
    }

    p = calloc(1, size + TOTAL_OVERHEAD_LEN);
    if (!p)
    {
        return NULL;
    }

    p_chunk = p + HEAD_OVERHEAD_LEN;

    *((size_t*)p) = size;
    memcpy(p + sizeof(size_t), HEAD_GUARD, 8);
    memcpy(p_chunk + size, TAIL_GUARD, 8);

    return p_chunk;
}

void *urealloc(void *ptr, size_t size)
{
    size_t chunk_size = 0;
    size_t *p_chunk_size = NULL;
    char *p_head_guard = NULL;
    char *p_tail_guard = NULL;

    if (!ptr)
    {
        return umalloc(size);
    }

    if (!size)
    {
        ufree(ptr);
        return NULL;
    }

    get_chunk_attrs(ptr, &p_chunk_size, &p_head_guard, &p_tail_guard);

    chunk_size = *p_chunk_size;
    if (size <= chunk_size)
    {
        *p_chunk_size = size;
        p_tail_guard = p_tail_guard - (chunk_size - size);
        memcpy(p_tail_guard, TAIL_GUARD, 8);
    }
    else
    {
        char *p = umalloc(size);
        if (p)
        {
            memcpy(p, ptr, chunk_size);
            ufree(ptr);
            ptr = p;
        }
        else
        {
            errno = ENOMEM;
            ptr = NULL;
        }
    }
    return ptr;
}

void ufree(void *ptr)
{
    size_t *p_chunk_size = NULL;
    char *p_head_guard = NULL;
    char *p_tail_guard = NULL;

    if (!ptr)
    {
        return;
    }

    get_chunk_attrs(ptr, &p_chunk_size, &p_head_guard, &p_tail_guard);

    /* Reset chunk_size & *guard values to 0. If 
       double-free pointer, it will triger above assert.*/
    memset(p_chunk_size, 0, HEAD_OVERHEAD_LEN);
    memset(p_tail_guard, 0, TAIL_OVERHEAD_LEN);

    free(p_chunk_size);
    return;
}

