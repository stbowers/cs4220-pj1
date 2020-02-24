#ifndef __CRC_H__
#define __CRC_H__

#include <bitstream.h>

typedef struct {
    BitStream *frame_stream;
    size_t frame_bits;
} CRCFrame;

/// Encodes the given bitstream into a new crc frame
CRCFrame* crc_encode(const BitStream *input, const BitStream *generator);

/// Free memory allocated for a crc frame
void crc_destroy(CRCFrame *frame);

/// Tests all hamming functions
void crc_test();

#endif // __CRC_H__
