/**
 * @file galois_field.h
 * @brief GF(2^8) arithmetic operations for QR code Reed-Solomon encoding.
 *
 * All operations use the irreducible polynomial x^8 + x^4 + x^3 + x^2 + 1
 * (0x11D), as specified by the QR code standard (ISO/IEC 18004).
 */
 
#ifndef GALOIS_FIELD_H
#define GALOIS_FIELD_H
 
#include <stdint.h>
 
/**
 * @brief Adds two GF(256) elements (XOR).
 * @param a First operand.
 * @param b Second operand.
 * @return a XOR b.
 */
uint8_t gf_add(uint8_t a, uint8_t b);
 
/**
 * @brief Multiplies two GF(256) elements using the Russian Peasant algorithm.
 *
 * Uses carry-less multiplication reduced by 0x11D.
 *
 * @param a First operand.
 * @param b Second operand.
 * @return Product of a and b in GF(256).
 */
uint8_t gf_mul(uint8_t a, uint8_t b);
 
/**
 * @brief Divides two GF(256) elements.
 *
 * Computes a * gf_inverse(b). Returns 0 if b is 0.
 *
 * @param a Dividend.
 * @param b Divisor (must be nonzero for a meaningful result).
 * @return a / b in GF(256), or 0 if b == 0.
 */
uint8_t gf_div(uint8_t a, uint8_t b);
 
/**
 * @brief Computes the multiplicative inverse of a GF(256) element.
 *
 * Uses Fermat's Little Theorem: a^(-1) = a^254 in GF(256).
 *
 * @param a Element to invert (must be nonzero).
 * @return Multiplicative inverse of a in GF(256).
 */
uint8_t gf_inverse(uint8_t a);
 
/**
 * @brief Raises a GF(256) element to a non-negative integer power.
 *
 * @param base Base element.
 * @param exp  Non-negative exponent.
 * @return base^exp in GF(256).
 */
uint8_t gf_pow(uint8_t base, int exp);
 
#endif // GALOIS_FIELD_H
 