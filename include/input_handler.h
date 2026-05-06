#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "qr_capacity.h"

typedef struct UserInput {
    const char *data;
    qr_mode_t encoding;
    qr_ecl_t ecl;
    int data_length;
    int data_set;
    int encoding_set;
    int ecl_set;
} UserInput;

int parse_input(int argc, char* argv[], UserInput *args);

#endif // INPUT_PARSER_H
