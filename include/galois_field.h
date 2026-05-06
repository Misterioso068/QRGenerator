#ifndef GALOIS_FIELD_H
#define GALOIS_FIELD_H

#include <stdint.h>

uint8_t gf_add(uint8_t a, uint8_t b);
uint8_t gf_mul(uint8_t a, uint8_t b);
uint8_t gf_div(uint8_t a, uint8_t b);
uint8_t gf_inverse(uint8_t a);
uint8_t gf_pow(uint8_t base, int exp);

#endif // GALOID_FIELD_H