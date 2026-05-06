#ifndef QR_GENERATOR_H
#define QR_GENERATOR_H

#include <stdint.h>
#include "input_handler.h"

uint8_t** create_qr(const UserInput *userIn, int *out_size);

#endif // QR_GENERATOR_H