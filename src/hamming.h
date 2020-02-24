#ifndef __HAMMING_H__
#define __HAMMING_H__

#include <bitstream.h>

typedef struct {
    BitStream *frame_stream;
    size_t frame_bits, message_bits;
} HammingFrame;

/// Given a message length (in bits), msg_len, returns the number of bits required for the hamming encoding of
/// that message
size_t hamming_frame_length(size_t message_length);

/// Given the length of a hamming code (in bits), hamming_len, returs the number of message bits in the code
size_t hamming_message_length(size_t frame_length);

/// Encodes the given bitstream into a new hamming frame
HammingFrame* hamming_encode(BitStream *input);

/// Creates a hamming frame from a bit stream
HammingFrame* hamming_frame_from_stream(BitStream *stream);

/// Decodes the given hamming frame into a bitstream
/// NOTE! Skips parity bits. User should call hamming_fix_errors first!
BitStream* hamming_decode(HammingFrame *frame);

/// Given a hamming frame, fixes any error found (up to 1 bit of error)
void hamming_fix_errors(HammingFrame *frame);

/// Free memory allocated for a hamming frame
void hamming_destroy();

/// Tests all hamming functions
void hamming_test();

#endif // __HAMMING_H__
