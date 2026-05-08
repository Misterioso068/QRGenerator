 /**
 * @file poly_math.h
 * @brief Polynomial arithmetic over GF(256).
 *
 * Polynomials are stored with index 0 as the highest degree coefficient.
 * For example, x^2 + x + 1 is represented as {1, 1, 1}.
 */
 
#ifndef POLY_MATH_H
#define POLY_MATH_H
 
#include <stdint.h>
 
/**
 * @brief Polynomial with coefficients in GF(256).
 *
 * Coefficients are stored MSB-first: coef[0] is the highest degree term
 * and coef[coef_count - 1] is the constant term.
 */
typedef struct Polynomial {
    uint8_t *coef;   /**< Heap-allocated coefficient array. */
    int coef_count;  /**< Number of coefficients (degree + 1). */
} Polynomial;
 
/**
 * @brief Creates a polynomial from a byte array.
 * @param bytes      Coefficient array (index 0 = highest degree).
 * @param byte_count Number of coefficients.
 * @return New Polynomial. Caller must call destroy_poly() when done.
 */
Polynomial create_poly(uint8_t *bytes, int byte_count);
 
/**
 * @brief Frees the memory associated with a polynomial.
 * @param p Pointer to the Polynomial to destroy.
 */
void destroy_poly(Polynomial *p);
 
/**
 * @brief Adds two polynomials over GF(256).
 * @param a First operand.
 * @param b Second operand.
 * @return Sum polynomial. Caller must call destroy_poly() when done.
 */
Polynomial poly_add(const Polynomial *a, const Polynomial *b);
 
/**
 * @brief Multiplies two polynomials over GF(256).
 * @param a First operand.
 * @param b Second operand.
 * @return Product polynomial. Caller must call destroy_poly() when done.
 */
Polynomial poly_mul(const Polynomial *a, const Polynomial *b);
 
/**
 * @brief Scales all coefficients of a polynomial by a GF(256) scalar.
 * @param a      Input polynomial.
 * @param scalar GF(256) scalar multiplier.
 * @return Scaled polynomial. Caller must call destroy_poly() when done.
 */
Polynomial poly_scale(const Polynomial *a, uint8_t scalar);
 
/**
 * @brief Divides polynomial a by polynomial b over GF(256).
 *
 * Returns only the remainder. Leading zero coefficients are trimmed.
 * If deg(a) < deg(b), returns a copy of a unchanged.
 *
 * @param a Dividend polynomial.
 * @param b Divisor polynomial.
 * @return Remainder polynomial. Caller must call destroy_poly() when done.
 */
Polynomial poly_div(const Polynomial *a, const Polynomial *b);
 
#endif // POLY_MATH_H