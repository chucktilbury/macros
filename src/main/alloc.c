
#include "common.h"

void* _mem_alloc(size_t size) {

    void* ptr = malloc(size);
    if(ptr == NULL)
        FATAL("out of memory: %lu bytes", size);

    memset(ptr, 0, size);
    return ptr;
}

void* _mem_realloc(void* ptr, size_t size) {

    void* p = realloc(ptr, size);
    if(p == NULL)
        FATAL("out of memory: %lu bytes", size);

    return p;
}

void* _mem_copy(void* optr, size_t size) {

    void* nptr = malloc(size);
    if(nptr == NULL)
        FATAL("cannot allocate to copy %lu bytes", size);

    memcpy(nptr, optr, size);
    return nptr;
}

char* _mem_copy_string(const char* str) {

    size_t len;
    if(str != NULL)
        len = strlen(str) + 1;
    else
        len = 1;

    char* ptr = malloc(len);
    if(ptr == NULL)
        FATAL("cannot allocate %lu bytes for string", len);

    if(str != NULL)
        memcpy(ptr, str, len);
    else
        ptr[0] = '\0';

    return ptr;
}

void _mem_free(void* ptr) {

    ASSERT(ptr != NULL, "attempt to free a NULL pointer");
    free(ptr);
}
