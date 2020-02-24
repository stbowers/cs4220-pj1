#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdlib.h>

typedef struct {
    unsigned char *bytes;
    size_t length;
} BitStream;

/// Creates a new bitstream filled with 0
BitStream *bitstream_create(size_t len);
/// Creates a new bitstream with a given length, and a copy of the given bitstream
BitStream *bitstream_copy(const BitStream *stream, size_t length);
/// Creates a new bitstream by concatinating the given streams
BitStream *bitstream_concat(const BitStream *lhs, const BitStream *rhs);
/// Frees the memory allocated for a given bitstream
void bitstream_destroy(BitStream *stream);

unsigned char bitstream_get(const BitStream *stream, size_t bit);
void bitstream_set(BitStream *stream, size_t bit, unsigned char value);
void bitstream_toggle(BitStream *stream, size_t bit);
void bitstream_sum(BitStream *stream, size_t bit, unsigned char value);

/// Calculates if the stream lhs represents a smaller binary number than rhs
int bitstream_lt(const BitStream *lhs, const BitStream *rhs);
/// Performs a bitwise logical left shift on the stream
void bitstream_sll(BitStream *stream);
/// Performs a bitwise logical right shift on the stream
void bitstream_srl(BitStream *stream);
/// Performs a bitwise xor on lhs, given the rhs argument
void bitstream_xor(BitStream *lhs, const BitStream *rhs);

void bitstream_read_from_string(BitStream *stream, const char *str);
void bitstream_write_to_string(BitStream *stream, char *str);

/// Tests all bitstream functions
void bitstream_test();

#endif // __BITSTREAM_H__
