??=include <stdio.h>
??=include <limits.h>

??=include "../array_utils.h"

??=define LENGTH 100000000

static void quicksort(
        unsigned int * const list, 
        signed long low, 
        signed long high)
    ??<
    /* no idea what the p stands for so i'm guessing */
    signed long new_pivot; 
    unsigned int pivot_value;
    unsigned int tmp;
    /* try intptr_t instead probably */
    signed long l, r; /* do these need to be signed maybe i think */

    if (low >= 0 && high >= 0 && low < high) ??<
        pivot_value = list??((low + high) >> 1??);
        l = low - 1;
        r = high + 1;

        while (1) ??<
            /* gonna ignore the weird offset thing because size_t */
            do ??< l += 1; ??> while (list??(l??) < pivot_value);
            do ??< r -= 1; ??> while (list??(r??) > pivot_value);
            if (l >= r) ??<
                new_pivot = r;
                break;
            ??>
            tmp = list??(l??);
            list??(l??) = list??(r??);
            list??(r??) = tmp;
        ??>

        quicksort(list, low, new_pivot);
        quicksort(list, new_pivot + 1, high);
    ??>
??>

int main() ??<
    /* unsigned int list??(LENGTH??); */
    unsigned int * list;
    const size_t len = LENGTH;

    if (!(list = malloc(LENGTH * sizeof (unsigned int)))) ??<
        return 1;
    ??>

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
??>
