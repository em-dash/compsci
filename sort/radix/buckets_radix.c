#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define LIST_LENGTH 128

struct bucket {
    unsigned int values[LIST_LENGTH];
    size_t count;
};

int main(int argc, char ** argv) {
    unsigned int list[LIST_LENGTH];
    struct bucket buckets[256];
    unsigned char byte;
    size_t idx;
    size_t i, j, k;

    /* generate list of LIST_LENGTH random ints */
    srand(time(NULL));
    for (i = 0; i < LIST_LENGTH; i++) {
        list[i] = rand();
    }
    printf("unsorted list:\n");
    for (i = 0; i < LIST_LENGTH; i++) {
        printf("%10d\n", list[i]);
    }
    printf("\n");

    /* initialize buckets */
    for (i = 0; i < 256; i++) {
        buckets[i].count = 0;
    }

    /* sort the list */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < LIST_LENGTH; j++) {
            byte = (list[j] >> (i * 8)) & 255;
            /* printf("sorting item no %d %.8x into bucket %.2x.  it's the %dth thing " */
            /*     "in that bucket\n", j, list[j], byte, */
            /*     buckets[byte].count + 1); */
            buckets[byte].values[buckets[byte].count] = list[j];
            buckets[byte].count += 1;
        }

        idx = 0;
        /* for each byte bucket */
        for (j = 0; j < 256; j++) {
            /* put the contents of the bucket back in the list */
            for (k = 0; k < buckets[j].count; k++) {
                list[idx] = buckets[j].values[k];
                idx += 1;
            }
            buckets[j].count = 0;
        }
        /* printf("sorted byte %d\n", i); */
        /* for (j = 0; j < LIST_LENGTH; j++) { */
        /*     printf("%8x\n", list[j]); */
        /* } */
        /* printf("\n"); */
    }

    printf("sorted list:\n");
    for (i = 0; i < LIST_LENGTH; i++) {
        printf("%10d\n", list[i]);
    }
    printf("\n");
}
