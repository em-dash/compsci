#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "array_utils.h"

void randomize_array(unsigned int * const array, size_t const len) {
    size_t i;

    srand(time(NULL));
    for (i = 0; i < len; i++) {
        array[i] = rand();
    }
}

void randomize_array_mod(
        unsigned int * const array,
        size_t const len, 
        unsigned int const mod) 
{
    size_t i;

    srand(time(NULL));
    for (i = 0; i < len; i++) {
        array[i] = rand() % mod;

    }
}

void sorted_array(unsigned int * const array, size_t const len) {
    size_t i;
    for (i = 0; i < len; i++) array[i] = i;
}

void oops_all_array(
        unsigned int * const array,
        size_t const len,
        unsigned int const oops) 
{
    size_t i;
    for (i = 0; i < len; i++) array[i] = oops;
}

void print_array(unsigned int * const array, size_t const len) {
    size_t i;

    for (i = 0; i < len; i++) {
        printf("%10u\n", array[i]);
    }
}
