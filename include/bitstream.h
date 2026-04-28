#ifndef BITSTREAM_H
#define BITSTREAM_H

typedef struct BitStream {
    uint8_t *data;
    size_t capacity;
    size_t bit_len;
} BitStream;

bitstream_create(size_t capacity);
bitstream_push_bit(Bitstream *bs, );
bitstream_push_bits();

#endif // BITSTREAM_H