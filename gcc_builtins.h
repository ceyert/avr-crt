#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "types.h"

typedef struct {
    int16_t quotient;
    int16_t remainder;
} divmodhi4_result;

extern uint16_t __umulhisi3(uint16_t a, uint16_t b);

extern divmodhi4_result __divmodhi4(int16_t a, int16_t b);

extern void memset(uint8_t *base_addr, uint8_t value, int size);

#endif // MATH_UTIL_H
