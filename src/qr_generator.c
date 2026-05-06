#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "qr_generator.h"
#include "qr_capacity.h"
#include "qr_format.h"
#include "poly_math.h"
#include "rs_encoding.h"
#include "logger.h"

static void write_bits(uint8_t *buf, int *bit_pos, int value, int num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
        if (value & (1 << i)) {
            buf[*bit_pos / 8] |= (1 << (7 - (*bit_pos % 8)));
        }
        (*bit_pos)++;
    }
}

static void print_bits(uint8_t *buf, int num_bits) {
    for (int i = 0; i < num_bits; i++) {
        int byte_index = i / 8;
        int bit_index  = 7 - (i % 8);
        int bit = (buf[byte_index] >> bit_index) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

static void print_hex(uint8_t *buf, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

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
            break;

        case QR_MODE_ALPHANUMERIC:
            write_bits(codewords, &bit_pos, 0b0010, 4);
            if (version >= 1 && version <= 9) 
                write_bits(codewords, &bit_pos, userIn->data_length, 9);
            else if (version >= 10 && version <= 26)
                write_bits(codewords, &bit_pos, userIn->data_length, 11);
            else
                write_bits(codewords, &bit_pos, userIn->data_length, 13);
            break;

        case QR_MODE_BYTE:
            write_bits(codewords, &bit_pos, 0b0100, 4);
            if (version >= 1 && version <= 9) 
                write_bits(codewords, &bit_pos, userIn->data_length, 8);
            else
                write_bits(codewords, &bit_pos, userIn->data_length, 16);
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

    // Write Data Bits
    for (int i = 0; userIn->data[i] != '\0'; i++) {
        write_bits(codewords, &bit_pos, userIn->data[i], 8);
    }

    // Write Terminator Bits
    int bits_leftover = (total_codewords * 8) - bit_pos;
    int terminator_bits = bits_leftover >= 4 ? 4 : bits_leftover;
    write_bits(codewords, &bit_pos, 0, terminator_bits);

    // Bit padding - pad to byte boundary
    while (bit_pos % 8 != 0) {
        write_bits(codewords, &bit_pos, 0, 1);
    }

    // Byte padding - fill remaining codewords
    int pad_byte = 0;
    while (bit_pos / 8 < total_codewords) {
        write_bits(codewords, &bit_pos, pad_byte ? 0x11 : 0xEC, 8);
        pad_byte = !pad_byte;
    }

    return codewords;
}

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
    int final_size = data_cw + ec_cw;
    *out_final_size = final_size;

    uint8_t *final = malloc(final_size);
    memcpy(final, codewords, data_cw);

    for (int i = 0; i < total_blocks; i++) {
        int block_size = (i < block_info->g1_blocks)
                       ? block_info->g1_data
                       : block_info->g2_data;

        Polynomial ec = reed_solomon_ecc(blocks[i], block_size, block_info->ec_per_block);
        memcpy(final + data_cw + (i * ec_cw), ec.coef, ec_cw);
        destroy_poly(&ec);
    }

    free(blocks);
    return final;
}

static uint8_t** create_grid(int version, int *out_size) {
    int size = (version * 4) + 17;
    *out_size = size;

    uint8_t **grid = malloc(size * sizeof(*grid));
    for (int i = 0; i < size; i++) {
        grid[i] = calloc(size, sizeof(*grid[i]));
    }

    return grid;
}

static void print_grid(uint8_t **grid, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
}

static void place_finder(uint8_t **grid, int row, int col) {
    // outer ring - 7x7 dark
    for (int i = 0; i < 7; i++) {
        grid[row][col + i]     = 1;  // top
        grid[row + 6][col + i] = 1;  // bottom
        grid[row + i][col]     = 1;  // left
        grid[row + i][col + 6] = 1;  // right
    }

    // middle ring - 5x5 light
    for (int i = 1; i < 6; i++) {
        grid[row + 1][col + i] = 0;  // top
        grid[row + 5][col + i] = 0;  // bottom
        grid[row + i][col + 1] = 0;  // left
        grid[row + i][col + 5] = 0;  // right
    }

    // inner square - 3x3 dark
    for (int r = 2; r < 5; r++) {
        for (int c = 2; c < 5; c++) {
            grid[row + r][col + c] = 1;
        }
    }
}

static void place_dummy_format_bits(uint8_t **grid, int size) {
    // Dummy Format bits
    int value = 0;
    for (int col = 0; col < 6; col++) {
        grid[8][col] = value;
    }
    grid[8][7] = 0;
    grid[8][8] = 0;
    grid[7][8] = 0;
    for (int row = 0; row < 6; row++) {
        grid[row][8] = value;
    }
    for (int col = size - 8; col < size; col++) {
        grid[8][col] = value;
    }
    for (int row = size - 7; row < size; row++) {
        grid[row][8] = value;
    }
}

static void place_dark_module(uint8_t **grid, int size, int version) {
    // Dark module
    int row = (4 * version) + 9;
    int col = 8;
    grid[row][col] = 1;
}

static void mark_visited(uint8_t **visited, int size, int version) {
    // top left finder + separator (8x8)
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            visited[r][c] = 1;

    // top right finder + separator
    for (int r = 0; r < 8; r++)
        for (int c = size - 8; c < size; c++)
            visited[r][c] = 1;

    // bottom left finder + separator
    for (int r = size - 8; r < size; r++)
        for (int c = 0; c < 8; c++)
            visited[r][c] = 1;

    // timing patterns
    for (int i = 8; i < size - 8; i++) {
        visited[6][i] = 1;
        visited[i][6] = 1;
    }

    // format info areas
    for (int i = 0; i < 9; i++) {
        visited[8][i] = 1;
        visited[i][8] = 1;
    }
    for (int i = size - 8; i < size; i++) {
        visited[8][i] = 1;
        visited[i][8] = 1;
    }

    // dark module
    visited[size - 8][8] = 1;
}

// places finder patterns, timing, dark module etc
static void place_function_patterns(uint8_t **grid, uint8_t **visited, int size, int version) {
    // Draw timing patterns first
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

    // Draw finders top left, top right, bottom right
    place_finder(grid, 0, 0);           // top left
    place_finder(grid, 0, size - 7);    // top right
    place_finder(grid, size - 7, 0);    // bottom left

    // Draw dummy format bits
    place_dummy_format_bits(grid, size);

    // Place Dark Module
    place_dark_module(grid, size, version);
}

static void place_data(uint8_t **grid, uint8_t **visited, int size, uint8_t *final, int final_size) {
    int bit_count = 0;
    int num_bits = final_size * 8;
    int going_up = 1;

    for (int right_col = size - 1; right_col >= 1; right_col -= 2) {
        if (right_col == 6) right_col--;  // skip timing column

        int row_start = going_up ? size - 1 : 0;
        int row_end   = going_up ? -1 : size;
        int row_step  = going_up ? -1 : 1;

        for (int row = row_start; row != row_end; row += row_step) {
            for (int dc = 0; dc < 2; dc++) {  // dc=0 is right col, dc=1 is left col
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

// applies masking and format info
static void apply_mask(uint8_t **grid, uint8_t **visited, int size, int mask) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (!visited[row][col] && mask_condition(mask, row, col)) {
                grid[row][col] ^= 1;  // flip the bit
            }
        }
    }
}

static void place_format_string(uint8_t **grid, int size, qr_ecl_t ecl, int mask) {
    const char *fmt = QR_FORMAT_STRINGS[ecl][mask];

    // Location 1 - around top left finder
    // horizontal: row 8, cols 0-8 (skipping col 6)
    int col_positions[] = {0, 1, 2, 3, 4, 5, 7, 8};  // skip col 6
    for (int i = 0; i < 6; i++) {
        grid[8][col_positions[i]] = fmt[14 - i] - '0';
    }
    grid[8][7] = fmt[8] - '0';
    grid[8][8] = fmt[7] - '0';

    // vertical: rows 0-8 col 8 (skipping row 6)
    int row_positions[] = {0, 1, 2, 3, 4, 5, 7, 8};  // skip row 6
    for (int i = 0; i < 6; i++) {
        grid[row_positions[i]][8] = fmt[14 - i] - '0';
    }
    grid[7][8] = fmt[8] - '0';
    grid[8][8] = fmt[7] - '0';

    // Location 2 - top right and bottom left
    // top right: row 8, cols size-8 to size-1
    for (int i = 0; i < 8; i++) {
        grid[8][size - 8 + i] = fmt[i] - '0';
    }

    // bottom left: rows size-7 to size-1, col 8
    for (int i = 0; i < 7; i++) {
        grid[size - 7 + i][8] = fmt[14 - i] - '0';
    }
}

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

    uint8_t **grid = create_grid(version, out_size);
    uint8_t **visited = create_grid(version, out_size); // grid to mark visited nodes

    mark_visited(visited, *out_size, version);
    place_function_patterns(grid, visited, *out_size, version);
    place_data(grid, visited, *out_size, final, final_size);

    int mask = 3;
    apply_mask(grid, visited, *out_size, mask);
    place_format_string(grid, *out_size, userIn->ecl, mask);

    print_bits(codewords, total_codewords * 8);
    print_hex(codewords, total_codewords);
    print_bits(final, final_size * 8);
    print_hex(final, final_size);
    printf("Version: %d, Total codewords: %d\n", version, total_codewords);
    printf("QR Capacity: %d\n", qr_get_capacity(version, userIn->ecl, userIn->encoding));
    printf("EC Per block: %d\n", block_info->ec_per_block);
    print_grid(grid, *out_size);

    free(codewords);
    free(final);
    return grid;
}