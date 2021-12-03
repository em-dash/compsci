#include "types.h"

#define VEC_TYPE(type) \
struct vec_##type { \
    type * data; \
    size_t len; \
    size_t maxlen; \
}; \
\
struct vec_##type * vapnd_##type(struct vec_##type * const v, type item) { \
    if (v.len < v.maxlen) { \
        v.data[len] = item; \
        len++; \
        return v; \
    } \
    /* TODO handle errors from vxtnd */\
    vxtnd_##type(v); \
    v.data[len] = item; \
    return v; \
} \
\
struct vec_##type * vxtnd_##type(struct vec_##type * const v) { \
    size_t i = 2; \
\
    while (i < v.len) { \
        i = i * 2; \
    } \
    v.len = i * 2; \
    return realloc(v.data, v.len * sizeof (type)); \
} \
