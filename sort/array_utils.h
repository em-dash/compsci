#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <stdlib.h>

void randomize_array(unsigned int * const list, size_t const len);
void randomize_array_mod(
        unsigned int * const array,
        size_t const len, 
        unsigned int const mod);
void oops_all_array(
        unsigned int * const array,
        size_t const len,
        unsigned int const oops);
void print_array(unsigned int * const array, size_t const len);
void sorted_array(unsigned int * const array, size_t const len);
void time_taken(struct timeval const t1);

#endif
