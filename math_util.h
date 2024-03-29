#ifndef MATH_UTIL_H
#define MATH_UTIL_H

// Assuming int is 16 bits for this platform
typedef signed int int16_t;  
typedef unsigned int uint16_t;

typedef struct {
    int16_t quotient;
    int16_t remainder;
} divmodhi4_result;

// Function prototypes
unsigned int __umulhisi3(unsigned int a, unsigned int b);
divmodhi4_result __divmodhi4(int16_t a, int16_t b);

#endif // MATH_UTIL_H
