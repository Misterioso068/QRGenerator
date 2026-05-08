 /**
 * @file rs_encoding.h
 * @brief Reed-Solomon error correction encoding for QR codes.
 */
 
#ifndef RS_ENCODING_H
#define RS_ENCODING_H
 
#include "poly_math.h"
 
/**
 * @brief Generates the Reed-Solomon generator polynomial for n EC codewords.
 *
 * Constructs g(x) = (x + α^0)(x + α^1)...(x + α^(n-1)) where α = 2 in GF(256).
 *
 * @param n Degree of the generator polynomial (number of EC codewords).
 * @return Generator polynomial of degree n. Caller must call destroy_poly().
 */
Polynomial generate_poly(int n);
 
/**
 * @brief Computes Reed-Solomon error correction codewords for a data block.
 *
 * The result is always padded to exactly ec_len bytes, preserving any
 * leading zero coefficients that would otherwise be trimmed by poly_div().
 *
 * @param data     Input data codewords.
 * @param data_len Number of data codewords.
 * @param ec_len   Number of EC codewords to generate.
 * @return Polynomial containing exactly ec_len EC codewords.
 *         Caller must call destroy_poly() when done.
 */
Polynomial reed_solomon_ecc(const uint8_t *data, int data_len, int ec_len);
 
#endif // RS_ENCODING_H