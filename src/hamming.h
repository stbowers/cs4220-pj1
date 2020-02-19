#ifndef __HAMMING_H__
#define __HAMMING_H__

#include <bitstream.h>

typedef struct {
    unsigned char *bytes;
} BitStream;

unsigned char bitstream_get(BitStream *stream, size_t bit);
void bitstream_set(BitStream *stream, size_t bit, unsigned char value);
void bitstream_toggle(BitStream *stream, size_t bit);

typedef struct {
    BitStream frame_bits;
    size_t frame_bits, message_bits;
} HammingFrame;

size_t hamming_frame_length(size_t message_length);
size_t hamming_message_length(size_t frame_length);

HammingFrame* hamming_encode(BitStream input);
void hamming_decode();

#endif // __HAMMING_H__
