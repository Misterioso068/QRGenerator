#ifndef POLY_MATH_H
#define POLY_MATH_H

#include <stdint.h>

/*
    Polynomial struct assumes coef[0] is highest degree and coef[N] is constant
    EX: [x^2, x, 1]
*/
typedef struct Polynomial {
    uint8_t *coef;
    int coef_count;
} Polynomial;

Polynomial create_poly(uint8_t *bytes, int byte_count);
void destroy_poly(Polynomial *p);

Polynomial poly_add(const Polynomial *a, const Polynomial *b);
Polynomial poly_mul(const Polynomial *a, const Polynomial *b);
Polynomial poly_scale(const Polynomial *a, uint8_t scalar);
Polynomial poly_div(const Polynomial *a, const Polynomial *b);

#endif // POLY_MATH_H