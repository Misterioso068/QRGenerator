/**
 * @file qr_format.h
 * @brief QR format information strings, version strings, and alignment positions.
 *
 * Contains precomputed lookup tables for:
 * - Format information strings (ECL + mask, all 32 combinations)
 * - Version information strings (versions 7-40)
 * - Alignment pattern center positions (versions 1-40)
 */
 
#ifndef QR_FORMAT_H
#define QR_FORMAT_H
 
#include <stddef.h>
#include "qr_capacity.h"
 
/**
 * @brief Format information strings indexed by [ecl][mask].
 *
 * Each string is 15 bits (MSB first), encoding the ECL and mask pattern
 * with BCH error correction, XORed with the mask pattern 101010000010010.
 * Order matches qr_ecl_t: L=0, M=1, Q=2, H=3.
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
 
/**
 * @brief Version information strings for versions 7-40.
 *
 * Each string is 18 bits (MSB first), encoding the version number with
 * BCH(18,6) error correction. Index 0 = version 7, index 33 = version 40.
 * Only required for QR versions 7 and above.
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
 
/**
 * @brief Returns the format information string for the given ECL and mask.
 * @param ecl  Error correction level.
 * @param mask Mask pattern index (0-7).
 * @return 15-character binary string (MSB first).
 */
static inline const char* qr_get_format_string(qr_ecl_t ecl, int mask) {
    return QR_FORMAT_STRINGS[ecl][mask];
}
 
/**
 * @brief Returns the version information string for the given version.
 * @param version QR version (must be >= 7).
 * @return 18-character binary string (MSB first), or NULL if version < 7 or > 40.
 */
static inline const char* qr_get_version_string(int version) {
    if (version < 7 || version > 40) return NULL;
    return QR_VERSION_STRINGS[version - 7];
}
 
/**
 * @brief Alignment pattern center coordinates per version.
 *
 * Each row lists the row/column coordinates used to generate all alignment
 * pattern center positions by taking every combination of (row[i], row[j]).
 * Positions that would overlap finder patterns must be skipped.
 * Terminated by 0. Version 1 has no alignment patterns ({0}).
 */
static const int QR_ALIGNMENT_POSITIONS[40][8] = {
    {0},                        /* version 1  - none */
    {6, 18, 0},                 /* version 2  */
    {6, 22, 0},                 /* version 3  */
    {6, 26, 0},                 /* version 4  */
    {6, 30, 0},                 /* version 5  */
    {6, 34, 0},                 /* version 6  */
    {6, 22, 38, 0},             /* version 7  */
    {6, 24, 42, 0},             /* version 8  */
    {6, 26, 46, 0},             /* version 9  */
    {6, 28, 50, 0},             /* version 10 */
    {6, 30, 54, 0},             /* version 11 */
    {6, 32, 58, 0},             /* version 12 */
    {6, 34, 62, 0},             /* version 13 */
    {6, 26, 46, 66, 0},         /* version 14 */
    {6, 26, 48, 70, 0},         /* version 15 */
    {6, 26, 50, 74, 0},         /* version 16 */
    {6, 30, 54, 78, 0},         /* version 17 */
    {6, 30, 56, 82, 0},         /* version 18 */
    {6, 30, 58, 86, 0},         /* version 19 */
    {6, 34, 62, 90, 0},         /* version 20 */
    {6, 28, 50, 72, 94, 0},     /* version 21 */
    {6, 26, 50, 74, 98, 0},     /* version 22 */
    {6, 30, 54, 78, 102, 0},    /* version 23 */
    {6, 28, 54, 80, 106, 0},    /* version 24 */
    {6, 32, 58, 84, 110, 0},    /* version 25 */
    {6, 30, 58, 86, 114, 0},    /* version 26 */
    {6, 34, 62, 90, 118, 0},    /* version 27 */
    {6, 26, 50, 74, 98, 122, 0},  /* version 28 */
    {6, 30, 54, 78, 102, 126, 0}, /* version 29 */
    {6, 26, 52, 78, 104, 130, 0}, /* version 30 */
    {6, 30, 56, 82, 108, 134, 0}, /* version 31 */
    {6, 34, 60, 86, 112, 138, 0}, /* version 32 */
    {6, 30, 58, 86, 114, 142, 0}, /* version 33 */
    {6, 34, 62, 90, 118, 146, 0}, /* version 34 */
    {6, 30, 54, 78, 102, 126, 150, 0}, /* version 35 */
    {6, 24, 50, 76, 102, 128, 154, 0}, /* version 36 */
    {6, 28, 54, 80, 106, 132, 158, 0}, /* version 37 */
    {6, 32, 58, 84, 110, 136, 162, 0}, /* version 38 */
    {6, 26, 54, 82, 110, 138, 166, 0}, /* version 39 */
    {6, 30, 58, 86, 114, 142, 170, 0}, /* version 40 */
};
 
#endif // QR_FORMAT_H