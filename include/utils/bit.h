#ifndef __BIT_H__
#define __BIT_H__

#include <common.h>

// get the value of a certain field
static inline uint32_t get_field(uint32_t n, uint32_t mask, uint32_t shift) {
    return (n & mask) >> shift;
}

// set the value of a certain field, then return the modified value
static inline uint32_t set_field(uint32_t n, uint32_t mask, uint32_t shift, uint32_t value) {
    n &= ~mask;
    n |= (value << shift) & mask;
    return n;
}

#endif
