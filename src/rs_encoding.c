#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "rs_encoding.h"
#include "galois_field.h"

#include <stdio.h>

Polynomial generate_poly(int n) {
    // g(x) = (x - α¹)(x - α²)(x - α³)...(x - αⁿ)
    int alpha = 2;

    uint8_t start[] = {1};
    Polynomial p = create_poly(start, 1);

    for (int i = 0; i < n; i++) {
        uint8_t factor[] = {1, gf_pow(alpha, i)};
        Polynomial temp = create_poly(factor, 2);
        Polynomial old = p;

        p = poly_mul(&p, &temp);

        destroy_poly(&temp);
        destroy_poly(&old);
    }

    return p;
}

Polynomial reed_solomon_ecc(const uint8_t *data, int data_len, int ec_len) {
    // step 1: create message poly with ec_len zeros appended
    uint8_t *padded = calloc(1, data_len + ec_len);
    memcpy(padded, data, data_len);

    Polynomial msg = create_poly(padded, data_len + ec_len);
    free(padded);

    // step 2: generate generator polynomial
    Polynomial gen = generate_poly(ec_len);

    // step 3: divide, remainder is EC codewords
    Polynomial remainder = poly_div(&msg, &gen);

    destroy_poly(&msg);
    destroy_poly(&gen);

    // pad remainder to exactly ec_len bytes
    if (remainder.coef_count < ec_len) {
        int diff = ec_len - remainder.coef_count;
        uint8_t *padded_coef = calloc(ec_len, 1);
        memcpy(padded_coef + diff, remainder.coef, remainder.coef_count);
        free(remainder.coef);
        remainder.coef = padded_coef;
        remainder.coef_count = ec_len;
    }

    return remainder;
}