#include <stdio.h>

#include "../array_utils.h"

#define LENGTH 10000

unsigned int balls = 0;

/* it seems like this will give up at a certain recursion level */
static void quicksort(
        unsigned int * const list, 
        signed long long low, 
        signed long long high)
    {
    /* no idea what the p stands for so i'm guessing */
    signed long long new_pivot; 
    unsigned int pivot_value;
    unsigned int tmp;
    signed long long l, r; /* do these need to be signed maybe i think */

    if (low >= 0 && high >= 0 && low < high) {
        pivot_value = list[(low + high) >> 1];
        l = low;
        r = high;

        while (1) {
            /* gonna ignore the weird offset thing because size_t */
            while (list[l] < pivot_value) l += 1;
            while (list[r] > pivot_value) r -= 1;
            if (l >= r) {
                new_pivot = r;
                break;
            }
            tmp = list[l];
            list[l] = list[r];
            list[r] = tmp;
        }

        quicksort(list, low, new_pivot);
        quicksort(list, new_pivot + 1, high);
    }
}

int main() {
    unsigned int list[LENGTH];
    const size_t len = LENGTH;

    randomize_array(list, len);
    /* sorted_array(list, len); */
    printf("unsorted list:\n");
    print_array(list, len);
    printf("\n");

    quicksort(list, 0, len - 1);
    printf("sorted list:\n");
    print_array(list, len);
}
