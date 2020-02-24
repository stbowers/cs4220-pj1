#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdlib.h>

typedef struct {
    unsigned char *bytes;
    size_t length;
} BitStream;

/// Creates a new bitstream filled with 0
BitStream *bitstream_create(size_t len);
/// Frees the memory allocated for a given bitstream
void bitstream_destroy(BitStream *stream);

unsigned char bitstream_get(BitStream *stream, size_t bit);
void bitstream_set(BitStream *stream, size_t bit, unsigned char value);
void bitstream_toggle(BitStream *stream, size_t bit);
void bitstream_sum(BitStream *stream, size_t bit, unsigned char value);

void bitstream_read_from_string(BitStream *stream, const char *str);
void bitstream_write_to_string(BitStream *stream, char *str);

/// Tests all bitstream functions
void bitstream_test();

#endif // __BITSTREAM_H__
