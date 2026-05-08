/**
 * @file input_handler.h
 * @brief Command-line argument parsing for the QR code generator.
 */
 
#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H
 
#include "qr_capacity.h"
 
/**
 * @brief Holds all parsed and validated user input.
 */
typedef struct UserInput {
    const char *data;    /**< Pointer to the data string to encode. */
    qr_mode_t encoding;  /**< Selected encoding mode. */
    qr_ecl_t ecl;        /**< Selected error correction level. */
    int data_length;     /**< Length of the data string in characters. */
    int data_set;        /**< Flag: 1 if -D was provided. */
    int encoding_set;    /**< Flag: 1 if -M was provided. */
    int ecl_set;         /**< Flag: 1 if -L was provided. */
} UserInput;
 
/**
 * @brief Parses command-line arguments into a UserInput struct.
 *
 * Accepts -M (encoding mode), -L (ECL), -D (data), and -- (data with dash prefix).
 * If -M auto is specified, encoding is auto-detected from the data string.
 * All three arguments (-M, -L, -D) are required.
 *
 * @param argc Argument count from main().
 * @param argv Argument vector from main().
 * @param args Pointer to UserInput struct to populate.
 * @return 0 on success, -1 on error (error printed to stderr).
 */
int parse_input(int argc, char* argv[], UserInput *args);
 
#endif // INPUT_HANDLER_H