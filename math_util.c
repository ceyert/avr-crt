#include "math_util.h"

unsigned int __umulhisi3(unsigned int a, unsigned int b) {
    unsigned long result = (unsigned long)a * (unsigned long)b;
    return (result >> 16) & 0xFFFF;
}

divmodhi4_result __divmodhi4(int16_t a, int16_t b) {
    divmodhi4_result result;
    uint16_t ua = (uint16_t)(a < 0 ? -a : a);
    uint16_t ub = (uint16_t)(b < 0 ? -b : b);
    uint16_t quotient = 0, remainder = 0;
    int quotientSign = (a < 0) ^ (b < 0) ? -1 : 1;
    
    for (int i = 15; i >= 0; --i) {
        remainder = (remainder << 1) | ((ua >> i) & 1);
        if (remainder >= ub) {
            remainder -= ub;
            quotient |= (1U << i);
        }
    }
    
    result.quotient = quotientSign * (int16_t)quotient;
    result.remainder = (a < 0) ? (int16_t)-remainder : (int16_t)remainder;
    return result;
}
