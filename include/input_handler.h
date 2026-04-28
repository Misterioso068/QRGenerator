#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

// Keep UserInput private
typedef struct UserInput {
    const char *encoding;
    const char *ecl;
    int encoding_set;
    int ecl_set;
} UserInput;

int parse_input(int argc, char* argv[], UserInput *args);

#endif // INPUT_PARSER_H
