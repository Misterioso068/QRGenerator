#include "galois_field.h"

uint8_t gf_add(uint8_t a, uint8_t b) {
    return a ^ b;
}

uint8_t gf_sub(uint8_t a, uint8_t b) {
    return a ^ b;
}

// Russian Peasant
uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t result = 0;

    while (b) {
        if (b & 1)
            result ^= a;

        uint8_t carry = a & 0x80;  // check MSB BEFORE shift

        a <<= 1;

        if (carry)
            a ^= 0x1D;  // reduced polynomial (0x11D without x^8)

        b >>= 1;
    }

    return result;
}

uint8_t gf_div(uint8_t a, uint8_t b) {
    if (b == 0) return 0;
    return gf_mul(a, gf_inverse(b));
}

// Fermat's Little Theorem
uint8_t gf_inverse(uint8_t a) {
    uint8_t result = a;
    for (int i = 0; i < 253; i++) {  // multiply a by itself 253 more times = a^254
        result = gf_mul(result, a);
    }
    return result;
}

uint8_t gf_pow(uint8_t base, int exp) {
    uint8_t result = 1;
    for (int i = 0; i < exp; i++)
        result = gf_mul(result, base);
    return result;
}