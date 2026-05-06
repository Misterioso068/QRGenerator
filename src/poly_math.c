#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "poly_math.h"
#include "galois_field.h"

Polynomial create_poly(uint8_t *bytes, int byte_count) {
    Polynomial p;
    p.coef = calloc(1, byte_count);
    if (!p.coef) {
        p.coef = NULL;
        p.coef_count = 0;
        return p;
    }

    memcpy(p.coef, bytes, byte_count);
    p.coef_count = byte_count;
    return p;
}

void destroy_poly(Polynomial *p) {
    free(p->coef);
    p->coef = NULL;
    p->coef_count = 0;
}

Polynomial poly_add(const Polynomial *a, const Polynomial *b) {
    if (a->coef_count < b->coef_count) return poly_add(b, a);

    int a_count = a->coef_count;
    int b_count = b->coef_count;
    int offset = a_count - b_count;

    Polynomial p;    
    p.coef = calloc(1, a_count);
    if (!p.coef) {
        p.coef = NULL;
        p.coef_count = 0;
        return p;
    }
    p.coef_count = a_count;

    if (offset > 0) {
        memcpy(p.coef, a->coef, offset);
    }

    for (int i = 0; i < b_count; i++) {
        p.coef[offset + i] = gf_add(a->coef[offset + i], b->coef[i]);
    }

    return p;
}

Polynomial poly_mul(const Polynomial *a, const Polynomial *b) {
    int a_count = a->coef_count;
    int b_count = b->coef_count;
    int new_count = a_count + b_count - 1;

    Polynomial p;
    p.coef = calloc(1, new_count);
    if (!p.coef) {
        p.coef = NULL;
        p.coef_count = 0;
        return p;
    }
    p.coef_count = new_count;

    for (int i = 0; i < a_count; i++) {
        for (int j = 0; j < b_count; j++) {
            p.coef[i + j] = gf_add(gf_mul(a->coef[i], b->coef[j]), p.coef[i + j]);
        }
    }

    return p;
}

Polynomial poly_scale(const Polynomial *a, uint8_t scalar) {
    int a_count = a->coef_count;

    Polynomial p;
    p.coef = calloc(1, a_count);
    if (!p.coef) {
        p.coef = NULL;
        p.coef_count = 0;
        return p;
    }
    p.coef_count = a_count;

    for (int i = 0; i < a_count; i++) {
        p.coef[i] = gf_mul(a->coef[i], scalar);
    }

    return p;
}

Polynomial poly_div(const Polynomial *a, const Polynomial *b) {
    // remainder starts as a copy of a
    // loop runs (a_count - b_count + 1) times, once per quotient term
    // each iteration:
    //   1. scalar = gf_div(remainder[0], b[0])  -- leading terms
    //   2. scaled = poly_scale(b, scalar)
    //   3. remainder = poly_add(remainder, scaled) -- cancels leading term
    //   4. remainder is now one degree shorter (leading term is 0/gone)
    // after loop, remainder holds the final remainder
    Polynomial p;
    p.coef = NULL;
    p.coef_count = 0;

    Polynomial remainder;
    remainder.coef = malloc(a->coef_count);
    if (!remainder.coef) {
        return p;
    }
    memcpy(remainder.coef, a->coef, a->coef_count);
    remainder.coef_count = a->coef_count;

    if (a->coef_count < b->coef_count) return remainder;

    int a_count = a->coef_count;
    int b_count = b->coef_count;

    for (int i = 0; i < a_count - b_count + 1; i++) {
        if (remainder.coef[i] == 0) continue;

        uint8_t multiplier = gf_div(remainder.coef[i], b->coef[0]);

        for (int j = 0; j < b_count; j++) {
            remainder.coef[i + j] = gf_add(remainder.coef[i + j], b->coef[j] == 0 ? 0 : gf_mul(b->coef[j], multiplier));
        }
    }

    int start = 0;
    while (start < remainder.coef_count && remainder.coef[start] == 0) start++;

    int trimmed_count = remainder.coef_count - start;
    p.coef = malloc(trimmed_count);
    if (!p.coef) {
        p.coef = NULL;
        p.coef_count = 0;
        return p;
    }
    memcpy(p.coef, remainder.coef + start, trimmed_count);
    p.coef_count = trimmed_count;

    destroy_poly(&remainder);
    return p;
}
