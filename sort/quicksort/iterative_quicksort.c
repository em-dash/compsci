#include <stdio.h>

#include "../array_utils.h"

#define LENGTH 100000000

struct qsort_frame {
    struct qsort_frame * prev;
    signed long low;
    signed long high;
};

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
    struct qsort_frame * stack;
    struct qsort_frame * stmp;

    if (!(stack = malloc(sizeof (struct qsort_frame)))) return -1;
    stack->high = high;
    stack->low = low;
    stack->prev = NULL;

    while (stack) {
        if (stack->low >= 0 && stack->high >= 0 && stack->low < stack->high) {
            pivot_value = list[(stack->low + stack->high) >> 1];
            l = stack->low - 1;
            r = stack->high + 1;

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
             * its prev at the current frame, and move the stack pointer to it.
             * */
            /* actually do the new frame first otherwise problems */
            if (!(stmp = malloc(sizeof (struct qsort_frame)))) return -1;
            stmp->low = stack-> low;
            stmp->high = new_pivot;
            stack->low = new_pivot + 1;
            /* stack->high = stack->high already */
            stmp->prev = stack;
            stack = stmp;

            /* quicksort(list, stack->low, new_pivot); */
            /* quicksort(list, new_pivot + 1, stack->high); */
        }
        else {
            stmp = stack;
            stack = stmp->prev;
            free(stmp);
        } /* will any of this work properly */
    }

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
