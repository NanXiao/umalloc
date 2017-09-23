#ifndef __UMALLOC__
#define __UMALLOC__

#ifdef __cplusplus
extern "C" {
#endif

void *umalloc(size_t size);
void ufree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __UMALLOC__ */
