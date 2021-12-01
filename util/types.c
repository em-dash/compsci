#include "types.h"

struct vec {
    void * data;
    size_t itemsz;
    size_t len;
    size_t maxlen;
};

/* if newsz is 0, size is rounded up to the next 2^n bytes, then doubled */
struct vec * vappend(struct vec * const v, void * item) {

    if (!newlen) {
        while (i < v.len) {
            i = i * 2;
        }
        return realloc(v, v.len = i * 2);
    }
}
