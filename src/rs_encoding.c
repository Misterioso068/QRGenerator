/**
 * @file rs_encoding.c
 * @brief Reed-Solomon error correction encoding for QR codes.
 *
 * This file implements the Reed-Solomon encoding process used
 * to generate QR code error correction codewords.
 *
 * The implementation operates over GF(256) and uses:
 * - Generator polynomial construction
 * - Polynomial multiplication
 * - Polynomial long division
 *
 * QR codes use Reed-Solomon ECC to recover damaged or missing data.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "rs_encoding.h"
#include "galois_field.h"

/**
 * @brief Generates a Reed-Solomon generator polynomial.
 *
 * Constructs the polynomial:
 *
 * g(x) = (x - α^0)(x - α^1)(x - α^2)...(x - α^(n-1))
 *
 * where α is the primitive element of GF(256).
 *
 * The resulting polynomial is used during polynomial division
 * to generate Reed-Solomon error correction codewords.
 *
 * @param n Number of error correction codewords.
 *
 * @return Polynomial Generator polynomial of degree n.
 *
 * @note The caller is responsible for freeing the returned
 *       polynomial using destroy_poly().
 */
Polynomial generate_poly(int n) {
    /*
     * Primitive element used by QR code GF(256).
     */
    int alpha = 2;

    /*
     * Start with the constant polynomial:
     * g(x) = 1
     */
    uint8_t start[] = {1};
    Polynomial p = create_poly(start, 1);

    /*
     * Multiply by each factor:
     * (x - α^i)
     */
    for (int i = 0; i < n; i++) {
        /*
         * In GF(256), subtraction is equivalent to addition,
         * so the factor becomes:
         * (x + α^i)
         */
        uint8_t factor[] = {1, gf_pow(alpha, i)};

        Polynomial temp = create_poly(factor, 2);

        /*
         * Store the previous polynomial so it can be freed
         * after multiplication.
         */
        Polynomial old = p;

        p = poly_mul(&p, &temp);

        destroy_poly(&temp);
        destroy_poly(&old);
    }

    return p;
}

/**
 * @brief Generates Reed-Solomon error correction codewords.
 *
 * Performs the standard Reed-Solomon encoding procedure:
 *
 * 1. Append ec_len zero bytes to the message polynomial
 * 2. Generate the Reed-Solomon generator polynomial
 * 3. Divide the padded message polynomial by the generator
 * 4. Use the remainder as the error correction codewords
 *
 * The resulting polynomial contains exactly ec_len bytes.
 *
 * @param data Input message data bytes.
 * @param data_len Number of data bytes.
 * @param ec_len Number of error correction codewords to generate.
 *
 * @return Polynomial Polynomial containing the error correction codewords.
 *
 * @note The caller is responsible for freeing the returned
 *       polynomial using destroy_poly().
 */
Polynomial reed_solomon_ecc(const uint8_t *data,int data_len, int ec_len) {
    /*
     * Step 1:
     * Create a padded message polynomial by appending
     * ec_len zero coefficients.
     */
    uint8_t *padded = calloc(1, data_len + ec_len);

    memcpy(padded, data, data_len);

    Polynomial msg = create_poly(padded, data_len + ec_len);

    free(padded);

    /*
     * Step 2:
     * Generate the Reed-Solomon generator polynomial.
     */
    Polynomial gen = generate_poly(ec_len);

    /*
     * Step 3:
     * Divide the padded message polynomial by the
     * generator polynomial.
     *
     * The remainder becomes the ECC codewords.
     */
    Polynomial remainder = poly_div(&msg, &gen);

    destroy_poly(&msg);
    destroy_poly(&gen);

    /*
     * Step 4:
     * Ensure the remainder contains exactly ec_len bytes.
     *
     * Leading zero coefficients may have been trimmed during
     * polynomial division, so pad them back if necessary.
     */
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