#include <stdio.h>

#include "../array_utils.h"

#define LENGTH 100000000

/* i need to make generics i know */
struct stack {
    struct qsort_frame * i;
    /* struct qsort_frame * top; */
    size_t top;
    size_t capacity;
};

struct qsort_frame {
    signed long low;
    signed long high;
};

static inline struct stack * grow_stack(struct stack * s) {
    /* if (stack.top - stack.i == stack.capacity) { */
    /*     stack.capacity *= 2; */
    /*     return realloc(stack.i, stack.capacity); */
    /* } */
    if (s->top == s->capacity) {
        s->capacity *= 2;
        return realloc(s->i, s->capacity);
    }
    return s;
}

static int quicksort(
        unsigned int * const list, 
        signed long low, 
        signed long high)
{
    /* no idea what the p stands for so i'm guessing */
    signed long new_pivot; 
    unsigned int pivot_value;
    unsigned int tmp;
    /* try intptr_t instead probably */
    signed long l, r; /* do these need to be signed maybe i think */
    struct stack s;

    s.capacity = 65536;
    s.top = 0;

    if (!(s.i = malloc(sizeof (struct qsort_frame) * s.capacity))) return -1;
    s.i[s.top].high = high;
    s.i[s.top].low = low;

    while (1) {
        if (s.i[s.top].low >= 0 && s.i[s.top].high >= 0 && s.i[s.top].low < s.i[s.top].high) {
            pivot_value = list[(s.i[s.top].low + s.i[s.top].high) >> 1];
            l = s.i[s.top].low - 1;
            r = s.i[s.top].high + 1;

            while (1) {
                /* why is this different from using while with l = low and 
                 * r = high? */
                do { l += 1; } while (list[l] < pivot_value);
                do { r -= 1; } while (list[r] > pivot_value);
                if (l >= r) {
                    new_pivot = r;
                    break;
                }
                tmp = list[l];
                list[l] = list[r];
                list[r] = tmp;
            }

            /* put the parameters of the high call into this frame.  create a
             * new frame, put the parameters of the low call into that, point
             * its prev at the current frame, and move the s pointer to it.
             * */
            if (!grow_stack(&s)) {
                free(s.i);
                return 1;
            }
            s.i[s.top + 1].low = s.i[s.top].low;
            s.i[s.top + 1].high = new_pivot;
            s.i[s.top].low = new_pivot + 1;
            /* s.top.high = s.top.high already */
            s.top += 1;

            /* quicksort(list, stack.top.low, new_pivot); */
            /* quicksort(list, new_pivot + 1, stack.top.high); */
        }
        else {
            if (s.top == 0) return 0;
            s.top -= 1;
        } /* will any of this work properly */
    }

    free(s.i);
    return 0;

    /* TODO have a goto cleanup here i guess */
}

int main() {
    /* unsigned int list[LENGTH]; */
    unsigned int * list;
    const size_t len = LENGTH;

    if (!(list = malloc(LENGTH * sizeof (unsigned int)))) {
        return 1;
    }

    randomize_array(list, len);
    /* randomize_array_mod(list, len, LENGTH); */
    /* oops_all_array(list, len, 69); */
    /* sorted_array(list, len); */

    /* printf("unsorted list:\n"); */
    /* print_array(list, len); */
    /* printf("\n"); */

    quicksort(list, 0, len - 1);
    /* printf("sorted list:\n"); */
    /* print_array(list, len); */
}
