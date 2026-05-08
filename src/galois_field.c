/**
 * @file galois_field.c
 * @brief Arithmetic operations for GF(256).
 *
 * This file implements finite field arithmetic used by the Reed-Solomon
 * error correction system in QR code generation.
 *
 * The field uses the irreducible polynomial:
 * x^8 + x^4 + x^3 + x^2 + 1 (0x11D)
 *
 * Operations are performed in GF(2^8), where:
 * - Addition/subtraction are XOR operations
 * - Multiplication is polynomial multiplication modulo 0x11D
 * - Division uses multiplicative inverses
 */

#include "galois_field.h"

/**
 * @brief Adds two values in GF(256).
 *
 * In GF(2^8), addition is equivalent to bitwise XOR.
 *
 * @param a First operand.
 * @param b Second operand.
 *
 * @return uint8_t Sum of a and b in GF(256).
 */
uint8_t gf_add(uint8_t a, uint8_t b) {
    return a ^ b;
}

/**
 * @brief Subtracts two values in GF(256).
 *
 * In GF(2^8), subtraction is identical to addition,
 * so this operation is also implemented using XOR.
 *
 * @param a First operand.
 * @param b Second operand.
 *
 * @return uint8_t Difference of a and b in GF(256).
 */
uint8_t gf_sub(uint8_t a, uint8_t b) {
    return a ^ b;
}

/**
 * @brief Multiplies two values in GF(256).
 *
 * Uses the Russian Peasant multiplication algorithm combined with
 * modular reduction using the QR code irreducible polynomial (0x11D).
 *
 * The reduction polynomial used during overflow handling is 0x1D,
 * which represents the lower 8 bits of 0x11D after removing x^8.
 *
 * @param a First operand.
 * @param b Second operand.
 *
 * @return uint8_t Product of a and b in GF(256).
 */
uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t result = 0;

    while (b) {
        /*
         * If the current bit of b is set,
         * add the current value of a to the result.
         */
        if (b & 1)
            result ^= a;

        /* Check for overflow before shifting left. */
        uint8_t carry = a & 0x80;

        a <<= 1;

        /*
         * Reduce using the irreducible polynomial if overflow occurred.
         */
        if (carry)
            a ^= 0x1D;

        b >>= 1;
    }

    return result;
}

/**
 * @brief Divides one value by another in GF(256).
 *
 * Division is performed by multiplying the dividend by the
 * multiplicative inverse of the divisor.
 *
 * @param a Dividend.
 * @param b Divisor.
 *
 * @return uint8_t Result of a / b in GF(256).
 *
 * @note Division by zero returns 0.
 */
uint8_t gf_div(uint8_t a, uint8_t b) {
    if (b == 0)
        return 0;

    return gf_mul(a, gf_inverse(b));
}

/**
 * @brief Computes the multiplicative inverse of a value in GF(256).
 *
 * Uses Fermat's Little Theorem:
 * a^254 = 1 for all nonzero a in GF(256)
 *
 * Therefore:
 * a^-1 = a^253
 *
 * @param a Value to invert.
 *
 * @return uint8_t Multiplicative inverse of a.
 *
 * @note The inverse of 0 is undefined.
 */
uint8_t gf_inverse(uint8_t a) {
    uint8_t result = a;

    /*
     * Multiply a by itself repeatedly:
     * a^254 = a * a^253
     */
    for (int i = 0; i < 253; i++) {
        result = gf_mul(result, a);
    }

    return result;
}

/**
 * @brief Raises a value to an integer power in GF(256).
 *
 * Performs repeated multiplication in the finite field.
 *
 * @param base Base value.
 * @param exp Exponent.
 *
 * @return uint8_t base raised to exp in GF(256).
 */
uint8_t gf_pow(uint8_t base, int exp) {
    uint8_t result = 1;

    for (int i = 0; i < exp; i++)
        result = gf_mul(result, base);

    return result;
}