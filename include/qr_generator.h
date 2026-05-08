/**
 * @file qr_generator.h
 * @brief QR code generation pipeline public interface.
 *
 * Provides the main QR generation entry point and encoding mode
 * auto-detection. Includes the alphanumeric character value lookup table.
 */
 
#ifndef QR_GENERATOR_H
#define QR_GENERATOR_H
 
#include <stdint.h>
#include "input_handler.h"
 
/**
 * @brief Lookup table mapping ASCII characters to QR alphanumeric values.
 *
 * Indexed as ALPHANUMERIC_TABLE[char - ' ']. Returns -1 for characters
 * not in the QR alphanumeric charset (0-9, A-Z, space, $ % * + - . / :).
 */
static const int ALPHANUMERIC_TABLE[] = {
    36, -1, -1, -1, 37, 38, -1, -1, -1, -1, 39, 40, -1, 41, 42, 43,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 44, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};
 
/**
 * @brief Generates a complete QR code grid from user input.
 *
 * Runs the full pipeline: version selection, data encoding, Reed-Solomon
 * ECC, matrix construction, zigzag data placement, mask selection, and
 * format/version string placement.
 *
 * @param userIn   Validated user input (data, encoding mode, ECL).
 * @param out_size Set to the grid width/height in modules on return.
 * @return Heap-allocated 2D grid (1 = dark, 0 = light), or NULL on failure.
 *         Caller must free each row and then the array itself.
 */
uint8_t** create_qr(const UserInput *userIn, int *out_size);
 
/**
 * @brief Detects the most compact QR encoding mode for the given string.
 *
 * Checks in order: numeric (0-9 only), alphanumeric (0-9, A-Z, space,
 * $%*+-./: ), then byte (any data). Returns the first mode that can
 * represent all characters.
 *
 * @param data Null-terminated input string.
 * @return QR_MODE_NUMERIC, QR_MODE_ALPHANUMERIC, or QR_MODE_BYTE.
 */
qr_mode_t detect_encoding(const char *data);
 
#endif // QR_GENERATOR_H