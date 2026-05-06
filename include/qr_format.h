#ifndef QR_FORMAT_H
#define QR_FORMAT_H

#include <stddef.h>
#include "qr_capacity.h"

/* Format information strings [4 ECL levels][8 mask patterns]
 * Order: L=0, M=1, Q=2, H=3 (matches qr_ecl_t enum)
 * Bits are MSB first
 */
static const char *QR_FORMAT_STRINGS[4][8] = {
    /* L */
    {"111011111000100", "111001011110011", "111110110101010", "111100010011101",
     "110011000101111", "110001100011000", "110110001000001", "110100101110110"},
    /* M */
    {"101010000010010", "101000100100101", "101111001111100", "101101101001011",
     "100010111111001", "100000011001110", "100111110010111", "100101010100000"},
    /* Q */
    {"011010101011111", "011000001101000", "011111100110001", "011101000000110",
     "010010010110100", "010000110000011", "010111011011010", "010101111101101"},
    /* H */
    {"001011010001001", "001001110111110", "001110011100111", "001100111010000",
     "000011101100010", "000001001010101", "000110100001100", "000100000111011"},
};

/* Version information strings [versions 7-40]
 * Index 0 = version 7, index 1 = version 8, etc.
 * Only needed for version 7+
 */
static const char *QR_VERSION_STRINGS[34] = {
    "000111110010010100",  /* version 7  */
    "001000010110111100",  /* version 8  */
    "001001101010011001",  /* version 9  */
    "001010010011010011",  /* version 10 */
    "001011101111110110",  /* version 11 */
    "001100011101100010",  /* version 12 */
    "001101100001000111",  /* version 13 */
    "001110011000001101",  /* version 14 */
    "001111100100101000",  /* version 15 */
    "010000101101111000",  /* version 16 */
    "010001010001011101",  /* version 17 */
    "010010101000010111",  /* version 18 */
    "010011010100110010",  /* version 19 */
    "010100100110100110",  /* version 20 */
    "010101011010000011",  /* version 21 */
    "010110100011001001",  /* version 22 */
    "010111011111101100",  /* version 23 */
    "011000111011000100",  /* version 24 */
    "011001000111100001",  /* version 25 */
    "011010111110101011",  /* version 26 */
    "011011000010001110",  /* version 27 */
    "011100110000011010",  /* version 28 */
    "011101001100111111",  /* version 29 */
    "011110110101110101",  /* version 30 */
    "011111001001010000",  /* version 31 */
    "100000100111010101",  /* version 32 */
    "100001011011110000",  /* version 33 */
    "100010100010111010",  /* version 34 */
    "100011011110011111",  /* version 35 */
    "100100101100001011",  /* version 36 */
    "100101010000101110",  /* version 37 */
    "100110101001100100",  /* version 38 */
    "100111010101000001",  /* version 39 */
    "101000110001101001",  /* version 40 */
};

/* Helper to get format string */
static inline const char* qr_get_format_string(qr_ecl_t ecl, int mask) {
    return QR_FORMAT_STRINGS[ecl][mask];
}

/* Helper to get version string (only valid for version >= 7) */
static inline const char* qr_get_version_string(int version) {
    if (version < 7 || version > 40) return NULL;
    return QR_VERSION_STRINGS[version - 7];
}

#endif // QR_FORMAT_H