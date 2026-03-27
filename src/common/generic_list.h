#ifndef _GENERIC_LIST_H_
#define _GENERIC_LIST_H_

#include "alloc.h"


/*
    Include this in a source file (not a header) and use the functions
    public by re-defining them and changing the names.

    (n) is the name to use
    (t) is the type, which is expected to be a type of pointer.
*/

#define _PTR_LIST_T(n, t) \
typedef struct n ##_list_t { \
    unsigned int cap; \
    unsigned int len; \
    t* buffer; \
} n ##_list_t; \
static inline n##_list_t* create_##n##_list(void) { \
    n ##_list_t* ptr = _ALLOC_TYPE(n ##_list_t);\
    ptr->len = 0; \
    ptr->cap = 1 << 3; \
    ptr->buffer = _ALLOC_ARRAY(t, ptr->cap); \
    return ptr; \
} \
static inline void destroy_##n##_list(n##_list_t* ptr) {\
    if(ptr != ((void*)0)) { \
        _FREE(ptr->buffer); \
        _FREE(ptr); \
    } \
} \
static inline void resize_##n##_list(n##_list_t* lst, int num) {\
    if(lst->len+num > lst->cap) { \
        while(lst->len+num > lst->cap) \
            lst->cap <<= 1; \
        lst->buffer = _REALLOC_ARRAY(lst->buffer, t, lst->cap);\
    }\
} \
static inline int normalize_##n##_list_index(n##_list_t* lst, int idx) {\
    /* i.e. idx = -1 referrs to the last item */ \
    int i = (int)lst->len+idx; \
    if(i > 0 && i < (int)lst->len) \
        return i; \
    else \
        return -1; \
}\
static inline void append_##n##_list(n##_list_t* lst, t val) { \
    resize_##n##_list(lst, 1);\
    lst->buffer[lst->len] = val; \
    lst->len++; \
} \
static inline t get_##n##_list(n##_list_t* lst, int index) { \
    int idx = normalize_##n##_list_index(lst, index); \
    if(0 > idx) \
        return ((void*)0); \
    else \
        return lst->buffer[idx]; \
}\
\
\
static inline unsigned int len_##n##_list(n##_list_t* ptr) {\
    return lst->len; \
}\
static inline unsigned int cap_##n##_list(n##_list_t* ptr) {\
    return lst->cap; \
}\
static inline t* raw_##n##_list(n##_list_t* ptr) {\
    return lst->buffer; \
}\
\
\
static inline void clear_##n##_list(n##_list_t* ptr) {\
    lst->len = 0;\
}\
static inline void push_##n##_list(n##_list_t* ptr, t val) {\
    append_##n##_list(ptr, val);\
}\
static inline t pop_##n##_list(n##_list_t* ptr) {\
    t val = ((void*)0); \
    if(lst->len > 0) {\
        val = lst->buffer[lst->len-1]; \
        lst->len--; \
    } \
    return val; \
}\
static inline t peek_##n##_list(n##_list_t* ptr) {\
    t val = ((void*)0); \
    if(lst->len > 0) \
        val = lst->buffer[lst->len-1]; \
    return val; \
}

#endif /* _GENERIC_LIST_H_ */
