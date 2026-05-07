#ifndef QR_GENERATOR_H
#define QR_GENERATOR_H

#include <stdint.h>
#include "input_handler.h"

static const int ALPHANUMERIC_TABLE[] = {
    36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

uint8_t** create_qr(const UserInput *userIn, int *out_size);
qr_mode_t detect_encoding(const char *data);

#endif // QR_GENERATOR_H