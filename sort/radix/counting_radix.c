#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/* if you make this number really big you can make it segfault cause the stack
 * frame won't fit in memory >:3 */
#define LIST_LENGTH 83789

int main(int argc, char ** argv) {
    unsigned int list_in[LIST_LENGTH];
    unsigned int list_out[LIST_LENGTH];
    /* i don't think size_t is correct cause the prefix sum part technically
     * could overflow it */
    size_t counts[256] = {0};
    unsigned char byte;
    size_t i, j;

    /* generate list of LIST_LENGTH random ints */
    srand(time(NULL));
    for (i = 0; i < LIST_LENGTH; i++) {
        list_in[i] = rand();
    }
    printf("unsorted list:\n");
    for (i = 0; i < LIST_LENGTH; i++) {
        printf("%10d\n", list_in[i]);
    }
    printf("\n");

    /* sort the list */
    for (i = 0; i < 4; i += 1) {
        for (j = 0; j < LIST_LENGTH; j += 1) {
            byte = (list_in[j] >> (i * 8)) & 255;
            counts[byte] += 1;
        }

        /* prefix sum.  idk how, but sum-how this works. */
        /* starts at 1 because index 0 doesn't get anything added */
        for (j = 1; j < 256; j += 1) {
            counts[j] += counts[j - 1];
        }

        /* for each list_in item, put it in list_out in the right place. start
         * from the highest index. */
        for (j = 0; j < LIST_LENGTH; j += 1) {
            /* not sure whether to store the bytes, maybe faster to just
             * calculate it again */
            byte = (list_in[LIST_LENGTH - j - 1] >> (i * 8)) & 255;
            counts[byte] -= 1;
            list_out[counts[byte]] = list_in[LIST_LENGTH - j - 1];
        }

        memset(&counts, 0, 256 * sizeof (size_t));

        memcpy(&list_in, &list_out, LIST_LENGTH * (sizeof (int)));
    }

    printf("sorted list:\n");
    for (i = 0; i < LIST_LENGTH; i++) {
        printf("%10d\n", list_in[i]);
    }
    printf("\n");
}
