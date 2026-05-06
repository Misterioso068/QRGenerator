#ifndef RS_ENCODING_H
#define RS_ENCODING_H

#include "poly_math.h"

Polynomial generate_poly(int n);
Polynomial reed_solomon_ecc(const uint8_t *data, int data_len, int ec_len);

#endif // RS_ENCODING_H