#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Warning: the guard string must be 8-byte length*/
#define HEAD_GUARD  "DEADBEEF"
#define TAIL_GUARD  "DEADBEEF"

#define HEAD_OVERHEAD_LEN   (sizeof(size_t) + 8)
#define TAIL_OVERHEAD_LEN   (8)
#define TOTAL_OVERHEAD_LEN  (HEAD_OVERHEAD_LEN + TAIL_OVERHEAD_LEN)

#define MAX_CHUNK_SIZE_LEN  (SIZE_MAX - TOTAL_OVERHEAD_LEN)

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

void ufree(void *ptr)
{
    size_t chunk_size = 0;
    char *p_chunk_size = NULL;
    char *p_head_guard = NULL;
    char *p_tail_guard = NULL;

    if (!ptr)
    {
        return;
    }

    p_chunk_size = ((char*)ptr) - HEAD_OVERHEAD_LEN;

    chunk_size = *((size_t*)p_chunk_size);
    p_head_guard = p_chunk_size + sizeof(size_t);
    p_tail_guard = ptr + chunk_size;

    /* Check up out-of-bound memory error */
    assert(!memcmp(p_head_guard, HEAD_GUARD, 8));

    /* Check down out-of-bound memory error */
    assert(!memcmp(p_tail_guard, TAIL_GUARD, 8));

    /* Reset chunk_size & *guard values to 0. If 
       double-free pointer, it will triger above assert.*/
    memset(p_chunk_size, 0, HEAD_OVERHEAD_LEN);
    memset(p_tail_guard, 0, TAIL_OVERHEAD_LEN);

    free(p_chunk_size);
    return;
}

