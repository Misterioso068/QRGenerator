/**
 * @file input_handler.c
 * @brief Command-line argument parsing and validation.
 *
 * This file handles:
 * - Parsing CLI arguments
 * - Validating QR encoding modes
 * - Validating error correction levels
 * - Populating the UserInput structure
 * - Automatic encoding mode detection
 *
 * Supported arguments:
 * - -M <mode>  : QR encoding mode
 * - -L <level> : Error correction level
 * - -D <data>  : Input data string
 * - -- <data>  : Alternate raw data input
 */

#include <string.h>

#include "input_handler.h"
#include "logger.h"
#include "qr_generator.h"

/**
 * @brief Validates a QR encoding mode string.
 *
 * Accepted encoding modes:
 * - numeric
 * - alphanumeric
 * - byte
 * - kanji
 * - auto
 *
 * @param mode Encoding mode string.
 *
 * @return int
 * @retval 0  Encoding mode is valid.
 * @retval -1 Encoding mode is invalid.
 */
static int validate_encoding_type_input(const char *mode) {
    if (strcmp(mode, "alphanumeric") == 0) return 0;
    if (strcmp(mode, "numeric") == 0)      return 0;
    if (strcmp(mode, "byte") == 0)         return 0;
    if (strcmp(mode, "kanji") == 0)        return 0;
    if (strcmp(mode, "auto") == 0)         return 0;

    return -1;
}

/**
 * @brief Validates a QR error correction level string.
 *
 * Accepted levels:
 * - L
 * - M
 * - Q
 * - H
 *
 * @param ecl Error correction level string.
 *
 * @return int
 * @retval 0  Error correction level is valid.
 * @retval -1 Error correction level is invalid.
 */
static int validate_ecl_input(const char *ecl) {
    if (strcmp(ecl, "L") == 0) return 0;
    if (strcmp(ecl, "M") == 0) return 0;
    if (strcmp(ecl, "Q") == 0) return 0;
    if (strcmp(ecl, "H") == 0) return 0;

    return -1;
}

/**
 * @brief Converts an encoding mode string into a qr_mode_t value.
 *
 * @param mode Encoding mode string.
 *
 * @return qr_mode_t Parsed QR encoding mode.
 *
 * @note Returns -1 if the mode is invalid.
 */
static qr_mode_t parse_encoding(const char *mode) {
    if (strcmp(mode, "numeric") == 0)
        return QR_MODE_NUMERIC;

    if (strcmp(mode, "alphanumeric") == 0)
        return QR_MODE_ALPHANUMERIC;

    if (strcmp(mode, "byte") == 0)
        return QR_MODE_BYTE;

    if (strcmp(mode, "kanji") == 0)
        return QR_MODE_KANJI;

    if (strcmp(mode, "auto") == 0)
        return QR_MODE_AUTO;

    return -1;
}

/**
 * @brief Converts an error correction level string into a qr_ecl_t value.
 *
 * @param ecl Error correction level string.
 *
 * @return qr_ecl_t Parsed QR error correction level.
 *
 * @note Returns -1 if the level is invalid.
 */
static qr_ecl_t parse_ecl(const char *ecl) {
    if (strcmp(ecl, "L") == 0) return QR_ECL_L;
    if (strcmp(ecl, "M") == 0) return QR_ECL_M;
    if (strcmp(ecl, "Q") == 0) return QR_ECL_Q;
    if (strcmp(ecl, "H") == 0) return QR_ECL_H;

    return -1;
}

/**
 * @brief Resets a UserInput structure to default values.
 *
 * Clears all flags and resets all parsed argument fields.
 *
 * @param args Pointer to the UserInput structure to reset.
 */
static void clear_args(UserInput *args) {

    /* Reset input structure fields. */
    args->data = NULL;

    args->encoding = -1;
    args->ecl = -1;

    args->data_length = 0;

    args->data_set = 0;
    args->encoding_set = 0;
    args->ecl_set = 0;
}

/**
 * @brief Parses command-line arguments into a UserInput structure.
 *
 * Expected usage:
 * @code
 * ./build/main -M byte -L M -D "hello world"
 * @endcode
 *
 * Required arguments:
 * - -M : Encoding mode
 * - -L : Error correction level
 * - -D : Input data
 *
 * If the encoding mode is set to "auto", the encoding
 * type is automatically detected from the input data.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @param args Output UserInput structure.
 *
 * @return int
 * @retval 0  Arguments parsed successfully.
 * @retval -1 Parsing or validation failed.
 */
int parse_input(int argc, char* argv[], UserInput *args) {

    clear_args(args);

    /*
     * At least one argument must be provided.
     */
    if (argc <= 1) {
        arguments_not_provided();
        return -1;
    }

    /*
     * Iterate through all command-line arguments.
     */
    for (int i = 1; i < argc; i++) {

        /*
         * Parse encoding mode argument.
         */
        if (strcmp(argv[i], "-M") == 0) {

            if (args->encoding_set) {
                duplicate_argument("-M");
                return -1;
            }

            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                missing_option_value("-M");
                return -1;
            }

            const char *encoding_type = argv[i + 1];

            if (validate_encoding_type_input(encoding_type) != 0) {
                invalid_encoding_type_argument(encoding_type);
                return -1;
            }

            args->encoding = parse_encoding(encoding_type);
            args->encoding_set = 1;

            i++;
        }

        /*
         * Parse error correction level argument.
         */
        else if (strcmp(argv[i], "-L") == 0) {

            if (args->ecl_set) {
                duplicate_argument("-L");
                return -1;
            }

            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                missing_option_value("-L");
                return -1;
            }

            const char *ecl = argv[i + 1];

            if (validate_ecl_input(ecl) != 0) {
                invalid_ecl_argument(ecl);
                return -1;
            }

            args->ecl = parse_ecl(ecl);
            args->ecl_set = 1;

            i++;
        }

        /*
         * Parse data argument.
         */
        else if (strcmp(argv[i], "-D") == 0) {

            if (args->data_set) {
                duplicate_argument("-D");
                return -1;
            }

            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                missing_option_value("-D");
                return -1;
            }

            args->data = argv[i + 1];
            args->data_length = strlen(argv[i + 1]);
            args->data_set = 1;

            i++;
        }

        /*
         * Alternate raw data input argument.
         */
        else if (strcmp(argv[i], "--") == 0) {

            if (args->data_set) {
                duplicate_argument("--");
                return -1;
            }

            if (i + 1 >= argc) {
                missing_option_value("--");
                return -1;
            }

            args->data = argv[i + 1];
            args->data_length = strlen(argv[i + 1]);
            args->data_set = 1;

            i++;
        }

        /*
         * Unknown argument encountered.
         */
        else {
            unknown_argument(argv[i]);
            return -1;
        }
    }

    /*
     * Ensure all required arguments were provided.
     */
    if (!args->encoding_set) {
        missing_required_argument("-M");
        return -1;
    }

    if (!args->ecl_set) {
        missing_required_argument("-L");
        return -1;
    }

    if (!args->data_set) {
        missing_required_argument("-D");
        return -1;
    }

    /*
     * Automatically detect encoding type if requested.
     */
    if (args->encoding == QR_MODE_AUTO) {
        args->encoding = detect_encoding(args->data);
    }

    return 0;
}