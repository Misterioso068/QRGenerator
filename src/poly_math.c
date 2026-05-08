/**
 * @file poly_math.c
 * @brief Polynomial arithmetic for Reed-Solomon encoding.
 *
 * This file implements polynomial operations over GF(256),
 * which are required for QR code Reed-Solomon error correction.
 *
 * Polynomial coefficients are stored in arrays where:
 * coef[0] corresponds to the highest-degree term.
 *
 * Example:
 * [3, 5, 1] represents:
 * 3x^2 + 5x + 1
 */

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "poly_math.h"
#include "galois_field.h"

/**
 * @brief Creates a polynomial from a byte array.
 *
 * Allocates memory for the coefficient array and copies
 * the provided coefficients into the new polynomial.
 *
 * @param bytes Array of polynomial coefficients.
 * @param byte_count Number of coefficients in the array.
 *
 * @return Polynomial Newly created polynomial.
 *
 * @note On allocation failure, the returned polynomial
 *       contains NULL coefficients and a count of 0.
 */
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

/**
 * @brief Frees memory associated with a polynomial.
 *
 * Resets the polynomial fields after freeing memory
 * to avoid dangling pointers.
 *
 * @param p Pointer to the polynomial to destroy.
 */
void destroy_poly(Polynomial *p) {
    free(p->coef);

    p->coef = NULL;
    p->coef_count = 0;
}

/**
 * @brief Adds two polynomials over GF(256).
 *
 * Polynomial addition in GF(256) is performed using XOR
 * on aligned coefficients.
 *
 * If the polynomials differ in degree, the smaller
 * polynomial is aligned to the least significant terms.
 *
 * @param a First polynomial.
 * @param b Second polynomial.
 *
 * @return Polynomial Result of a + b.
 *
 * @note On allocation failure, the returned polynomial
 *       contains NULL coefficients and a count of 0.
 */
Polynomial poly_add(const Polynomial *a, const Polynomial *b) {
    /*
     * Ensure 'a' is the larger polynomial so alignment
     * logic remains simple.
     */
    if (a->coef_count < b->coef_count)
        return poly_add(b, a);

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

    /* Copy unmatched leading coefficients. */
    if (offset > 0) {
        memcpy(p.coef, a->coef, offset);
    }

    /* Add aligned coefficients using GF addition (XOR). */
    for (int i = 0; i < b_count; i++) {
        p.coef[offset + i] = gf_add(a->coef[offset + i], b->coef[i]);
    }

    return p;
}

/**
 * @brief Multiplies two polynomials over GF(256).
 *
 * Uses standard polynomial multiplication where coefficient
 * arithmetic is performed in GF(256).
 *
 * @param a First polynomial.
 * @param b Second polynomial.
 *
 * @return Polynomial Result of a * b.
 *
 * @note On allocation failure, the returned polynomial
 *       contains NULL coefficients and a count of 0.
 */
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

    /*
     * Multiply every coefficient pair and accumulate
     * overlapping terms using GF addition.
     */
    for (int i = 0; i < a_count; i++) {
        for (int j = 0; j < b_count; j++) {
            p.coef[i + j] = gf_add(gf_mul(a->coef[i], b->coef[j]), p.coef[i + j]);
        }
    }

    return p;
}

/**
 * @brief Multiplies a polynomial by a scalar in GF(256).
 *
 * Each coefficient is multiplied independently by
 * the provided scalar value.
 *
 * @param a Polynomial to scale.
 * @param scalar Scalar multiplier.
 *
 * @return Polynomial Scaled polynomial.
 *
 * @note On allocation failure, the returned polynomial
 *       contains NULL coefficients and a count of 0.
 */
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

/**
 * @brief Computes the remainder of polynomial division over GF(256).
 *
 * Performs polynomial long division and returns the remainder.
 * This operation is heavily used during Reed-Solomon encoding
 * to generate error correction codewords.
 *
 * The algorithm repeatedly:
 * - Cancels the leading term
 * - Scales the divisor
 * - XORs the scaled divisor into the remainder
 *
 * @param a Dividend polynomial.
 * @param b Divisor polynomial.
 *
 * @return Polynomial Remainder of a / b.
 *
 * @note If the dividend degree is smaller than the divisor degree,
 *       a copy of the dividend is returned.
 *
 * @note On allocation failure, the returned polynomial
 *       contains NULL coefficients and a count of 0.
 */
Polynomial poly_div(const Polynomial *a, const Polynomial *b) {
    Polynomial p;
    p.coef = NULL;
    p.coef_count = 0;

    /*
     * Create a working copy of the dividend that will
     * be modified during division.
     */
    Polynomial remainder;

    remainder.coef = malloc(a->coef_count);
    if (!remainder.coef) {
        return p;
    }

    memcpy(remainder.coef, a->coef, a->coef_count);
    remainder.coef_count = a->coef_count;

    /*
     * If the dividend degree is already smaller,
     * the dividend itself is the remainder.
     */
    if (a->coef_count < b->coef_count)
        return remainder;

    int a_count = a->coef_count;
    int b_count = b->coef_count;

    /*
     * Polynomial long division.
     */
    for (int i = 0; i < a_count - b_count + 1; i++) {

        /* Skip already eliminated terms. */
        if (remainder.coef[i] == 0)
            continue;

        /*
         * Determine the multiplier needed to cancel
         * the current leading term.
         */
        uint8_t multiplier = gf_div(remainder.coef[i], b->coef[0]);

        /*
         * Subtract the scaled divisor from the remainder.
         * In GF(256), subtraction is XOR.
         */
        for (int j = 0; j < b_count; j++) {
            remainder.coef[i + j] = gf_add(remainder.coef[i + j], b->coef[j] == 0 ? 0 : gf_mul(b->coef[j], multiplier));
        }
    }

    /*
     * Remove leading zero coefficients from the remainder.
     */
    int start = 0;
    while (start < remainder.coef_count && remainder.coef[start] == 0) {
        start++;
    }

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