/**
 * @file qr_generator.c
 * @brief QR code generation pipeline.
 *
 * Implements the full QR code generation pipeline including data encoding,
 * Reed-Solomon error correction, matrix construction, masking, and format
 * string placement. Supports numeric, alphanumeric, and byte encoding modes
 * for QR versions 1-40 and all four error correction levels.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "qr_generator.h"
#include "qr_capacity.h"
#include "qr_format.h"
#include "poly_math.h"
#include "rs_encoding.h"
#include "logger.h"

/**
 * @brief Writes a value as a sequence of bits into a byte buffer, MSB first.
 *
 * @param buf      Destination byte buffer.
 * @param bit_pos  Pointer to the current bit position (updated on return).
 * @param value    Integer value to write.
 * @param num_bits Number of bits to write from value (MSB first).
 */
static void write_bits(uint8_t *buf, int *bit_pos, int value, int num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
        if (value & (1 << i)) {
            buf[*bit_pos / 8] |= (1 << (7 - (*bit_pos % 8)));
        }
        (*bit_pos)++;
    }
}

/**
 * @brief Prints a bit buffer to stdout as a binary string.
 * @internal
 * @param buf      Buffer to print.
 * @param num_bits Number of bits to print.
 */
static void print_bits(uint8_t *buf, int num_bits) {
    for (int i = 0; i < num_bits; i++) {
        int byte_index = i / 8;
        int bit_index  = 7 - (i % 8);
        int bit = (buf[byte_index] >> bit_index) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

/**
 * @brief Prints a byte buffer to stdout as space-separated hex values.
 * @internal
 * @param buf       Buffer to print.
 * @param num_bytes Number of bytes to print.
 */
static void print_hex(uint8_t *buf, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

/**
 * @brief Encodes data using QR numeric mode.
 *
 * Groups digits into sets of three and encodes each group as 10 bits.
 * Remaining pairs are encoded as 7 bits, and a single remaining digit
 * as 4 bits.
 *
 * @param codewords Destination buffer.
 * @param bit_pos   Pointer to the current bit position.
 * @param data      Null-terminated numeric string (digits 0-9 only).
 * @param length    Length of the data string.
 */
static void encode_numeric(uint8_t *codewords, int *bit_pos, const char *data, int length) {
    int i = 0;
    while (i + 2 < length) {
        int val = (data[i] - '0') * 100 + (data[i+1] - '0') * 10 + (data[i+2] - '0');
        write_bits(codewords, bit_pos, val, 10);
        i += 3;
    }
    if (i + 1 < length) {
        int val = (data[i] - '0') * 10 + (data[i+1] - '0');
        write_bits(codewords, bit_pos, val, 7);
        i += 2;
    }
    if (i < length) {
        write_bits(codewords, bit_pos, data[i] - '0', 4);
    }
}

/**
 * @brief Encodes data using QR byte mode.
 *
 * Each character is written as its raw 8-bit ASCII value.
 *
 * @param codewords Destination buffer.
 * @param bit_pos   Pointer to the current bit position.
 * @param data      Null-terminated string to encode.
 */
static void encode_byte(uint8_t *codewords, int *bit_pos, const char *data) {
    for (int i = 0; data[i] != '\0'; i++) {
        write_bits(codewords, bit_pos, data[i], 8);
    }
}

/**
 * @brief Encodes data using QR alphanumeric mode.
 *
 * Characters are encoded using the 45-character QR alphanumeric alphabet.
 * Pairs of characters are combined as (first * 45 + second) and encoded
 * as 11 bits. A single remaining character is encoded as 6 bits.
 *
 * Valid characters: 0-9, A-Z, space, $ % * + - . / :
 *
 * @param codewords Destination buffer.
 * @param bit_pos   Pointer to the current bit position.
 * @param data      Null-terminated alphanumeric string.
 * @param length    Length of the data string.
 */
static void encode_alphanumeric(uint8_t *codewords, int *bit_pos, const char *data, int length) {
    int i = 0;
    while (i + 1 < length) {
        int val = (45 * ALPHANUMERIC_TABLE[data[i] - ' ']) + ALPHANUMERIC_TABLE[data[i+1] - ' '];
        write_bits(codewords, bit_pos, val, 11);
        i += 2;
    }
    if (i < length) {
        write_bits(codewords, bit_pos, ALPHANUMERIC_TABLE[data[i] - ' '], 6);
    }
}

/**
 * @brief Encodes user data into QR codewords with mode indicator,
 *        character count, data bits, terminator, and padding.
 *
 * @param userIn          Pointer to the user input (data, encoding, length).
 * @param version         QR version number (1-40).
 * @param total_codewords Total number of data codewords for this version/ECL.
 * @return Heap-allocated byte array of encoded codewords, or NULL on failure.
 *         Caller must free() the returned buffer.
 */
static uint8_t* encode_data(const UserInput *userIn, int version, int total_codewords) {
    uint8_t *codewords = malloc(total_codewords * sizeof(*codewords));
    memset(codewords, 0, total_codewords);

    int bit_pos = 0;

    switch (userIn->encoding) {
        case QR_MODE_NUMERIC:
            write_bits(codewords, &bit_pos, 0b0001, 4);
            if (version >= 1 && version <= 9) 
                write_bits(codewords, &bit_pos, userIn->data_length, 10);
            else if (version >= 10 && version <= 26)
                write_bits(codewords, &bit_pos, userIn->data_length, 12);
            else
                write_bits(codewords, &bit_pos, userIn->data_length, 14);
            encode_numeric(codewords, &bit_pos, userIn->data, userIn->data_length);
            break;

        case QR_MODE_ALPHANUMERIC:
            write_bits(codewords, &bit_pos, 0b0010, 4);
            if (version >= 1 && version <= 9) 
                write_bits(codewords, &bit_pos, userIn->data_length, 9);
            else if (version >= 10 && version <= 26)
                write_bits(codewords, &bit_pos, userIn->data_length, 11);
            else
                write_bits(codewords, &bit_pos, userIn->data_length, 13);
            encode_alphanumeric(codewords, &bit_pos, userIn->data, userIn->data_length);
            break;

        case QR_MODE_BYTE:
            write_bits(codewords, &bit_pos, 0b0100, 4);
            if (version >= 1 && version <= 9) 
                write_bits(codewords, &bit_pos, userIn->data_length, 8);
            else
                write_bits(codewords, &bit_pos, userIn->data_length, 16);
            encode_byte(codewords, &bit_pos, userIn->data);
            break;

        case QR_MODE_KANJI:
            write_bits(codewords, &bit_pos, 0b1000, 4);
            if (version >= 1 && version <= 9) 
                write_bits(codewords, &bit_pos, userIn->data_length, 8);
            else if (version >= 10 && version <= 26)
                write_bits(codewords, &bit_pos, userIn->data_length, 10);
            else
                write_bits(codewords, &bit_pos, userIn->data_length, 12);
            break;

        default:
            encoding_type_not_provided();
            free(codewords);
            return NULL;
    }

    // terminator bits (up to 4 zero bits)
    int bits_leftover = (total_codewords * 8) - bit_pos;
    int terminator_bits = bits_leftover >= 4 ? 4 : bits_leftover;
    write_bits(codewords, &bit_pos, 0, terminator_bits);

    // bit padding to byte boundary
    while (bit_pos % 8 != 0) {
        write_bits(codewords, &bit_pos, 0, 1);
    }

    // byte padding with alternating 0xEC and 0x11
    int pad_byte = 0;
    while (bit_pos / 8 < total_codewords) {
        write_bits(codewords, &bit_pos, pad_byte ? 0x11 : 0xEC, 8);
        pad_byte = !pad_byte;
    }

    return codewords;
}

/**
 * @brief Splits codewords into blocks, computes EC codewords, and
 *        produces the final interleaved codeword sequence.
 *
 * For multi-block versions, data codewords and EC codewords are
 * interleaved across blocks as required by the QR specification.
 *
 * @param codewords      Encoded data codewords.
 * @param block_info     Block structure info for this version/ECL.
 * @param out_final_size Pointer set to the total size of the returned buffer.
 * @return Heap-allocated final codeword sequence (data + EC, interleaved).
 *         Caller must free() the returned buffer.
 */
static uint8_t* build_final_sequence(uint8_t *codewords, const qr_ec_block_info_t *block_info, int *out_final_size) {
    int total_blocks = block_info->g1_blocks + block_info->g2_blocks;
    uint8_t **blocks = malloc(total_blocks * sizeof(*blocks));

    int offset = 0;
    for (int i = 0; i < block_info->g1_blocks; i++) {
        blocks[i] = &codewords[offset];
        offset += block_info->g1_data;
    }
    for (int i = 0; i < block_info->g2_blocks; i++) {
        blocks[block_info->g1_blocks + i] = &codewords[offset];
        offset += block_info->g2_data;
    }

    int data_cw   = block_info->total_codewords;
    int ec_cw     = block_info->ec_per_block;
    int final_size = data_cw + (ec_cw * total_blocks);
    *out_final_size = final_size;

    uint8_t *final = malloc(final_size);
    int pos = 0;

    // interleave data codewords across blocks
    int max_block_size = (block_info->g2_blocks > 0) ? block_info->g2_data : block_info->g1_data;
    for (int byte = 0; byte < max_block_size; byte++) {
        for (int b = 0; b < total_blocks; b++) {
            int bsize = (b < block_info->g1_blocks) ? block_info->g1_data : block_info->g2_data;
            if (byte < bsize)
                final[pos++] = blocks[b][byte];
        }
    }

    // compute and interleave EC codewords across blocks
    Polynomial *ec_blocks = malloc(total_blocks * sizeof(Polynomial));
    for (int i = 0; i < total_blocks; i++) {
        int block_size = (i < block_info->g1_blocks) ? block_info->g1_data : block_info->g2_data;
        ec_blocks[i] = reed_solomon_ecc(blocks[i], block_size, ec_cw);
    }
    for (int byte = 0; byte < ec_cw; byte++) {
        for (int b = 0; b < total_blocks; b++) {
            final[pos++] = ec_blocks[b].coef[byte];
        }
    }
    
    for (int i = 0; i < total_blocks; i++) destroy_poly(&ec_blocks[i]);
    free(ec_blocks);
    free(blocks);
    return final;
}

/**
 * @brief Allocates a zeroed 2D QR module grid for the given version.
 *
 * Grid size is calculated as (version * 4) + 17. Each cell is a uint8_t
 * where 1 = dark module and 0 = light module.
 *
 * @param version  QR version number (1-40).
 * @param out_size Pointer set to the grid size (width = height).
 * @return Heap-allocated 2D grid. Caller must free each row and the array.
 */
static uint8_t** create_grid(int version, int *out_size) {
    int size = (version * 4) + 17;
    *out_size = size;

    uint8_t **grid = malloc(size * sizeof(*grid));
    for (int i = 0; i < size; i++) {
        grid[i] = calloc(size, sizeof(*grid[i]));
    }

    return grid;
}

/**
 * @brief Prints the QR grid to stdout as space-separated 0/1 values.
 * @internal
 * @param grid 2D grid to print.
 * @param size Grid width/height.
 */
static void print_grid(uint8_t **grid, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
}

/**
 * @brief Places a 7x7 finder pattern at the given top-left corner position.
 *
 * Finder patterns consist of a dark outer ring, a light middle ring,
 * and a dark 3x3 inner square.
 *
 * @param grid Grid to draw on.
 * @param row  Top-left row of the finder pattern.
 * @param col  Top-left column of the finder pattern.
 */
static void place_finder(uint8_t **grid, int row, int col) {
    for (int i = 0; i < 7; i++) {
        grid[row][col + i]     = 1;
        grid[row + 6][col + i] = 1;
        grid[row + i][col]     = 1;
        grid[row + i][col + 6] = 1;
    }

    for (int i = 1; i < 6; i++) {
        grid[row + 1][col + i] = 0;
        grid[row + 5][col + i] = 0;
        grid[row + i][col + 1] = 0;
        grid[row + i][col + 5] = 0;
    }

    for (int r = 2; r < 5; r++) {
        for (int c = 2; c < 5; c++) {
            grid[row + r][col + c] = 1;
        }
    }
}

/**
 * @brief Places a 5x5 alignment pattern centered at (cx, cy).
 *
 * Alignment patterns consist of a dark outer ring, a light inner ring,
 * and a single dark center module.
 *
 * @param grid Grid to draw on.
 * @param cx   Row of the center module.
 * @param cy   Column of the center module.
 */
static void place_alignment(uint8_t **grid, int cx, int cy) {
    for (int i = -2; i <= 2; i++) {
        grid[cx - 2][cy + i] = 1;
        grid[cx + 2][cy + i] = 1;
        grid[cx + i][cy - 2] = 1;
        grid[cx + i][cy + 2] = 1;
    }
    for (int r = -1; r <= 1; r++)
        for (int c = -1; c <= 1; c++)
            grid[cx + r][cy + c] = 0;
    grid[cx][cy] = 1;
}

/**
 * @brief Places all alignment patterns for the given QR version.
 *
 * Uses the alignment position lookup table from qr_format.h. Positions
 * that would overlap with finder patterns are skipped. Placed cells are
 * also marked in the visited grid.
 *
 * @param grid    Grid to draw on.
 * @param visited Visited grid to mark reserved cells.
 * @param size    Grid size.
 * @param version QR version number (1-40).
 */
static inline void place_alignments(uint8_t **grid, uint8_t **visited, int size, int version) {
    const int *pos = QR_ALIGNMENT_POSITIONS[version - 1];
    if (pos[0] == 0) return;

    for (int i = 0; pos[i] != 0; i++) {
        for (int j = 0; pos[j] != 0; j++) {
            int cx = pos[i];
            int cy = pos[j];

            if (cx <= 8 && cy <= 8) continue;
            if (cx <= 8 && cy >= size - 8) continue;
            if (cx >= size - 8 && cy <= 8) continue;

            place_alignment(grid, cx, cy);
            for (int r = -2; r <= 2; r++)
                for (int c = -2; c <= 2; c++)
                    visited[cx + r][cy + c] = 1;
        }
    }
}

/**
 * @brief Places zeroed dummy format information bits in their reserved positions.
 *
 * Reserves the format information areas around the finder patterns with
 * placeholder zero values. These are overwritten by place_format_string()
 * after masking.
 *
 * @param grid Grid to draw on.
 * @param size Grid size.
 */
static void place_dummy_format_bits(uint8_t **grid, int size) {
    int value = 0;
    for (int col = 0; col < 6; col++) grid[8][col] = value;
    grid[8][7] = 0;
    grid[8][8] = 0;
    grid[7][8] = 0;
    for (int row = 0; row < 6; row++) grid[row][8] = value;
    for (int col = size - 8; col < size; col++) grid[8][col] = value;
    for (int row = size - 7; row < size; row++) grid[row][8] = value;
}

/**
 * @brief Places the mandatory dark module at its fixed position.
 *
 * The dark module is always placed at row (4 * version + 9), column 8.
 *
 * @param grid    Grid to draw on.
 * @param size    Grid size (unused, kept for consistency).
 * @param version QR version number.
 */
static void place_dark_module(uint8_t **grid, int size, int version) {
    int row = (4 * version) + 9;
    int col = 8;
    grid[row][col] = 1;
}

/**
 * @brief Places the version information strings for QR versions 7 and above.
 *
 * Version information is placed in two 6x3 blocks: one in the bottom-left
 * area and one in the top-right area of the grid. Has no effect for
 * versions 1-6.
 *
 * @param grid    Grid to draw on.
 * @param size    Grid size.
 * @param version QR version number.
 */
static void place_version_string(uint8_t **grid, int size, int version) {
    if (version < 7) return;

    const char *ver = QR_VERSION_STRINGS[version - 7];

    for (int i = 0; i < 18; i++) {
        int row = (size - 11) + (i % 3);
        int col = i / 3;
        grid[row][col] = ver[17 - i] - '0';
    }

    for (int i = 0; i < 18; i++) {
        int row = i / 3;
        int col = (size - 11) + (i % 3);
        grid[row][col] = ver[17 - i] - '0';
    }
}

/**
 * @brief Marks all function pattern cells as visited (reserved).
 *
 * Marks finder patterns, separators, timing patterns, format information
 * areas, the dark module, and (for version 7+) version information areas
 * in the visited grid so that data placement skips them.
 *
 * @param visited Visited grid to mark.
 * @param size    Grid size.
 * @param version QR version number.
 */
static void mark_visited(uint8_t **visited, int size, int version) {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            visited[r][c] = 1;

    for (int r = 0; r < 8; r++)
        for (int c = size - 8; c < size; c++)
            visited[r][c] = 1;

    for (int r = size - 8; r < size; r++)
        for (int c = 0; c < 8; c++)
            visited[r][c] = 1;

    for (int i = 8; i < size - 8; i++) {
        visited[6][i] = 1;
        visited[i][6] = 1;
    }

    for (int i = 0; i < 9; i++) {
        visited[8][i] = 1;
        visited[i][8] = 1;
    }
    for (int i = size - 8; i < size; i++) {
        visited[8][i] = 1;
        visited[i][8] = 1;
    }

    visited[size - 8][8] = 1;

    if (version >= 7) {
        for (int r = size - 11; r < size - 8; r++)
            for (int c = 0; c < 6; c++)
                visited[r][c] = 1;
        for (int r = 0; r < 6; r++)
            for (int c = size - 11; c < size - 8; c++)
                visited[r][c] = 1;
    } 
}

/**
 * @brief Places all QR function patterns onto the grid.
 *
 * Draws timing patterns, finder patterns, alignment patterns, dummy
 * format bits, the dark module, and version information (if applicable).
 *
 * @param grid    Grid to draw on.
 * @param visited Visited grid for marking reserved cells.
 * @param size    Grid size.
 * @param version QR version number.
 */
static void place_function_patterns(uint8_t **grid, uint8_t **visited, int size, int version) {
    int value = 1;
    for (int col = 0; col < size; col++) {
        grid[6][col] = value;
        value = !value;
    }

    value = 1;
    for (int row = 0; row < size; row++) {
        grid[row][6] = value;
        value = !value;
    }

    place_finder(grid, 0, 0);
    place_finder(grid, 0, size - 7);
    place_finder(grid, size - 7, 0);

    place_alignments(grid, visited, size, version);
    place_dummy_format_bits(grid, size);
    place_dark_module(grid, size, version);

    if (version >= 7) place_version_string(grid, size, version);
}

/**
 * @brief Places data bits into the QR grid using the zigzag scan pattern.
 *
 * Scans the grid in 2-column wide strips from right to left, alternating
 * between upward and downward direction. Column 6 (timing pattern) is
 * skipped. Reserved cells in the visited grid are skipped.
 *
 * @param grid       Grid to place data into.
 * @param visited    Visited grid marking reserved cells.
 * @param size       Grid size.
 * @param final      Final interleaved codeword sequence.
 * @param final_size Number of bytes in the final sequence.
 */
static void place_data(uint8_t **grid, uint8_t **visited, int size, uint8_t *final, int final_size) {
    int bit_count = 0;
    int num_bits = final_size * 8;
    int going_up = 1;

    for (int right_col = size - 1; right_col >= 1; right_col -= 2) {
        if (right_col == 6) right_col--;

        int row_start = going_up ? size - 1 : 0;
        int row_end   = going_up ? -1 : size;
        int row_step  = going_up ? -1 : 1;

        for (int row = row_start; row != row_end; row += row_step) {
            for (int dc = 0; dc < 2; dc++) {
                int col = right_col - dc;
                if (visited[row][col]) continue;
                if (bit_count >= num_bits) break;

                int byte_index = bit_count / 8;
                int bit_index  = 7 - (bit_count % 8);
                grid[row][col] = (final[byte_index] >> bit_index) & 1;
                bit_count++;
            }
        }
        going_up = !going_up;
    }
}

/**
 * @brief Evaluates whether a given mask pattern applies to a module position.
 *
 * Returns nonzero if the mask condition is true for the given row and
 * column, per the QR code specification.
 *
 * @param mask Mask pattern index (0-7).
 * @param row  Module row.
 * @param col  Module column.
 * @return Nonzero if the mask applies, 0 otherwise.
 */
static int mask_condition(int mask, int row, int col) {
    switch (mask) {
        case 0: return (row + col) % 2 == 0;
        case 1: return row % 2 == 0;
        case 2: return col % 3 == 0;
        case 3: return (row + col) % 3 == 0;
        case 4: return (row / 2 + col / 3) % 2 == 0;
        case 5: return (row * col) % 2 + (row * col) % 3 == 0;
        case 6: return ((row * col) % 2 + (row * col) % 3) % 2 == 0;
        case 7: return ((row + col) % 2 + (row * col) % 3) % 2 == 0;
        default: return 0;
    }
}

/**
 * @brief Applies a mask pattern to all non-reserved data modules.
 *
 * Flips (XORs) each data module where the mask condition is true.
 * Can be applied twice to undo the mask (since XOR is its own inverse).
 *
 * @param grid    Grid to apply mask to.
 * @param visited Visited grid marking reserved cells (not masked).
 * @param size    Grid size.
 * @param mask    Mask pattern index (0-7).
 */
static void apply_mask(uint8_t **grid, uint8_t **visited, int size, int mask) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (!visited[row][col] && mask_condition(mask, row, col)) {
                grid[row][col] ^= 1;
            }
        }
    }
}

/**
 * @brief Calculates the penalty score for a masked QR grid.
 *
 * Evaluates all four QR penalty rules:
 * - Rule 1: Runs of 5+ same-color modules in rows/columns.
 * - Rule 2: 2x2 blocks of same-color modules.
 * - Rule 3: Finder-like patterns in rows/columns.
 * - Rule 4: Imbalance between dark and light modules.
 *
 * @param grid Grid to score.
 * @param size Grid size.
 * @return Total penalty score (lower is better).
 */
static int score_mask(uint8_t **grid, int size) {
    int penalty = 0;

    // rule 1: horizontal runs
    for (int i = 0; i < size; i++) {
        int j = 0;
        while (j < size) {
            int run = 1;
            while (j + run < size && grid[i][j + run] == grid[i][j]) run++;
            if (run >= 5) penalty += 3 + (run - 5);
            j += run;
        }
    }

    // rule 1: vertical runs
    for (int i = 0; i < size; i++) {
        int j = 0;
        while (j < size) {
            int run = 1;
            while (j + run < size && grid[j + run][i] == grid[j][i]) run++;
            if (run >= 5) penalty += 3 + (run - 5);
            j += run;
        }
    }

    // rule 2: 2x2 boxes
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1; j++) {
            uint8_t color = grid[i][j];
            if (grid[i][j + 1] == color &&
                grid[i + 1][j] == color &&
                grid[i + 1][j + 1] == color) {
                penalty += 3;
            }
        }
    }

    // rule 3: finder-like patterns
    int pattern1[] = {1,0,1,1,1,0,1,0,0,0,0};
    int pattern2[] = {0,0,0,0,1,0,1,1,1,0,1};
    for (int i = 0; i < size; i++) {
        for (int j = 0; j <= size - 11; j++) {
            int match1 = 1, match2 = 1;
            for (int k = 0; k < 11; k++) {
                uint8_t cell = grid[i][j + k];
                if (cell != pattern1[k]) match1 = 0;
                if (cell != pattern2[k]) match2 = 0;
            }
            if (match1 || match2) penalty += 40;
        }
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j <= size - 11; j++) {
            int match1 = 1, match2 = 1;
            for (int k = 0; k < 11; k++) {
                uint8_t cell = grid[j + k][i];
                if (cell != pattern1[k]) match1 = 0;
                if (cell != pattern2[k]) match2 = 0;
            }
            if (match1 || match2) penalty += 40;
        }
    }

    // rule 4: dark/light balance
    int dark_count = 0;
    int total_count = size * size;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (grid[i][j] == 1) dark_count++;

    int percent = (int)((float)dark_count / total_count * 100.0f);
    int prev5 = percent / 5 * 5;
    int next5 = prev5 + 5;
    int diff = (abs(prev5 - 50) < abs(next5 - 50)) ? abs(prev5 - 50) : abs(next5 - 50);
    penalty += (diff / 5) * 10;

    return penalty;
}

/**
 * @brief Evaluates all 8 mask patterns and applies the one with the lowest penalty.
 *
 * Each mask is applied, scored, and then unapplied (XOR is its own inverse).
 * The best mask is then applied permanently.
 *
 * @param grid    Grid to mask.
 * @param visited Visited grid marking reserved cells.
 * @param size    Grid size.
 * @return Index of the selected mask pattern (0-7).
 */
static int apply_best_mask(uint8_t **grid, uint8_t **visited, int size) {
    int best_mask = 0;
    int lowest_penalty = INT_MAX;
    for (int i = 0; i < 8; i++) {
        apply_mask(grid, visited, size, i);
        int penalty = score_mask(grid, size);
        if (penalty < lowest_penalty) {
            lowest_penalty = penalty;
            best_mask = i;
        }
        apply_mask(grid, visited, size, i);
    }
    apply_mask(grid, visited, size, best_mask);
    return best_mask;
}

/**
 * @brief Places the format information string into both reserved locations.
 *
 * Format information encodes the ECL and mask pattern. It is placed in
 * two locations for redundancy: around the top-left finder pattern
 * (location 1) and split between the top-right and bottom-left finder
 * patterns (location 2).
 *
 * @param grid Grid to place format info on.
 * @param size Grid size.
 * @param ecl  Error correction level.
 * @param mask Selected mask pattern index (0-7).
 */
static void place_format_string(uint8_t **grid, int size, qr_ecl_t ecl, int mask) {
    const char *fmt = QR_FORMAT_STRINGS[ecl][mask];

    // location 1 horizontal: row 8, cols 0-8 (skipping col 6)
    int col_positions[] = {0, 1, 2, 3, 4, 5, 7, 8};
    for (int i = 0; i < 8; i++) {
        grid[8][col_positions[i]] = fmt[i] - '0';
    }

    // location 1 vertical: col 8, rows 0-8 (skipping row 6)
    int row_positions[] = {0, 1, 2, 3, 4, 5, 7, 8};
    for (int i = 0; i < 8; i++) {
        grid[row_positions[i]][8] = fmt[14 - i] - '0';
    }

    // location 2 bottom-left: col 8, rows size-7 to size-1
    for (int i = 0; i < 8; i++) {
        grid[size - i - 1][8] = fmt[i] - '0';
    }

    // location 2 top-right: row 8, cols size-8 to size-1
    for (int i = 0; i < 8; i++) {
        grid[8][size - 8 + i] = fmt[7 + i] - '0';
    }
}

/**
 * @brief Automatically detects the most efficient QR encoding mode for the data.
 *
 * Tests each mode in order of efficiency (numeric > alphanumeric > byte)
 * and returns the most compact mode that can represent all characters.
 *
 * @param data Null-terminated input string.
 * @return QR_MODE_NUMERIC, QR_MODE_ALPHANUMERIC, or QR_MODE_BYTE.
 */
qr_mode_t detect_encoding(const char *data) {
    int can_numeric = 1;
    int can_alpha = 1;

    for (int i = 0; data[i] != '\0'; i++) {
        char c = data[i];

        if (c < '0' || c > '9') can_numeric = 0;

        if (!((c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'Z') ||
              c == ' ' || c == '$' || c == '%' ||
              c == '*' || c == '+' || c == '-' ||
              c == '.' || c == '/' || c == ':')) {
            can_alpha = 0;
        }
    }

    if (can_numeric) return QR_MODE_NUMERIC;
    if (can_alpha)   return QR_MODE_ALPHANUMERIC;
    return QR_MODE_BYTE;
}

/**
 * @brief Generates a complete QR code grid from user input.
 *
 * Executes the full QR generation pipeline:
 * 1. Determines the minimum QR version for the data.
 * 2. Encodes data into codewords.
 * 3. Computes and interleaves Reed-Solomon EC codewords.
 * 4. Builds the QR matrix with all function patterns.
 * 5. Places data bits via zigzag scan.
 * 6. Selects and applies the optimal mask pattern.
 * 7. Places format and version information strings.
 *
 * @param userIn   Pointer to the validated user input.
 * @param out_size Pointer set to the grid size (width = height in modules).
 * @return Heap-allocated 2D QR module grid (1 = dark, 0 = light),
 *         or NULL on failure. Caller must free each row and the array.
 */
uint8_t** create_qr(const UserInput *userIn, int *out_size) {
    int version = qr_min_version(userIn->data_length, userIn->ecl, userIn->encoding);
    if (version == -1) {
        int max = qr_get_capacity(40, userIn->ecl, userIn->encoding);
        data_too_long(userIn->data_length, max);
        return NULL;
    }

    const qr_ec_block_info_t *block_info = qr_get_ec_blocks(version, userIn->ecl);
    int total_codewords = block_info->total_codewords;

    uint8_t *codewords = encode_data(userIn, version, total_codewords);
    if (!codewords) return NULL;

    int final_size;
    uint8_t *final = build_final_sequence(codewords, block_info, &final_size);

    uint8_t **grid    = create_grid(version, out_size);
    uint8_t **visited = create_grid(version, out_size);

    mark_visited(visited, *out_size, version);
    place_function_patterns(grid, visited, *out_size, version);
    place_data(grid, visited, *out_size, final, final_size);
    
    int mask = apply_best_mask(grid, visited, *out_size);
    place_format_string(grid, *out_size, userIn->ecl, mask);

    // Debug prints
    //printf("mask: %d\n", mask);
    //print_bits(final, final_size * 8);
    //print_hex(final, final_size);
    //printf("Version: %d, Total codewords: %d\n", version, total_codewords);
    //printf("QR Capacity: %d\n", qr_get_capacity(version, userIn->ecl, userIn->encoding));
    //printf("EC Per block: %d\n", block_info->ec_per_block);
    //print_grid(grid, *out_size);

    for (int i = 0; i < *out_size; i++) free(visited[i]);
    free(visited);
    free(codewords);
    free(final);
    return grid;
}