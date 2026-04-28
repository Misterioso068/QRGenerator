#include <string.h>
#include "input_handler.h"
#include "logger.h"

static int validate_encoding_type_input(const char *mode) {
    if (strcmp(mode, "alphanumeric") == 0) return 0;
    if (strcmp(mode, "numeric") == 0)      return 0;
    if (strcmp(mode, "byte") == 0)         return 0;
    if (strcmp(mode, "kanji") == 0)        return 0;
    return -1;
}
static int validate_ecl_input(const char *ecl) {
    if (strcmp(ecl, "L") == 0) return 0;
    if (strcmp(ecl, "M") == 0) return 0;
    if (strcmp(ecl, "Q") == 0) return 0;
    if (strcmp(ecl, "H") == 0) return 0;
    return -1;
}

// Expected Input Example: ./build/main -M byte -L M
int parse_input(int argc, char* argv[], UserInput *args) {
    // Reset input struct
    args->encoding = NULL;
    args->ecl = NULL;
    args->encoding_set = 0;
    args->ecl_set = 0;

    if (argc <= 1) {
        arguments_not_provided();
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-M") == 0) {
            if (args->encoding_set) {
                duplicate_argument("-M");
                return -1;
            }

            if (i + 1 >= argc) {
                missing_option_value("-M");
                return -1;
            }

            const char *encoding_type = argv[i + 1];
            if (validate_encoding_type_input(encoding_type) != 0) {
                invalid_encoding_type_argument(encoding_type);
                return -1;
            }

            args->encoding = encoding_type;
            args->encoding_set = 1;
            i++;
        }
        else if (strcmp(argv[i], "-L") == 0) {
            if (args->ecl_set) {
                duplicate_argument("-L");
                return -1;
            }

            if (i + 1 >= argc) {
                missing_option_value("-L");
                return -1;
            }

            const char *ecl = argv[i + 1];
            if (validate_ecl_input(ecl) != 0) {
                invalid_ecl_argument(ecl);
                return -1;
            }

            args->ecl = ecl;
            args->ecl_set = 1;
            i++;
        }
        else {
            unknown_argument(argv[i]);
            return -1;
        }
    }

    if (!args->encoding_set) {
        missing_required_argument("-M");
        return -1;
    }

    if (!args->ecl_set) {
        missing_required_argument("-L");
        return -1;
    }

    return 0;
}