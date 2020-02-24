#include <assert.h>
#include <crc.h>
#include <stdio.h>
#include <string.h>

/// Encodes the given bitstream into a new crc frame
CRCFrame* crc_encode(const BitStream *input, const BitStream *generator) {
    CRCFrame *frame;
    BitStream *remainder;
    size_t i;

    // Allocate memory for a new frame
    frame = (CRCFrame*)malloc(sizeof(CRCFrame));

    // Make a copy of the input for doing calculations
    // Appending (generator->length - 1) 0s is required to make the calculation work,
    // and is equivalent to multiplying by the degree of the generator
    remainder = bitstream_copy(input, input->length + generator->length - 1);

    // Repeatedly subtract the generator from the remainder until we've gone through all
    // the bits of the input
    for (i = 0; i < input->length; i++) {
        // If the first bit is 0 we should skip the xor, until we get to the next 1
        if (bitstream_get(remainder, 0)) {
            // bitwise xor the generator from the remainder, which has the same effect as subtraction
            bitstream_xor(remainder, generator);
        }

        // Shift the remainder to the right once
        // Since the binary is being stored in reverse order, this is the
        // same as shifting the bit string to the left
        bitstream_srl(remainder);
    }

    // We only want generator->length - 1 bits in the checksum
    remainder->length = generator->length - 1; 

    // Create the frame bitstream by concatinating the input with the remainder
    frame->frame_bits = input->length + remainder->length;
    frame->frame_stream = bitstream_concat(input, remainder);
    
    // Clean up
    bitstream_destroy(remainder);

    return frame;
}

/// Free memory allocated for a crc frame
void crc_destroy(CRCFrame *frame) {
    free(frame->frame_stream);
}

/// Tests all hamming functions
void crc_test() {
    char *str, output[75];
    BitStream *input, *generator;
    CRCFrame *frame;

    printf("  => Testing CRC functions\n");

    str = "10011101";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    str = "1001";
    generator = bitstream_create(strlen(str));
    bitstream_read_from_string(generator, str);
    frame = crc_encode(input, generator);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "10011101100"));
    crc_destroy(frame);
    bitstream_destroy(input);
    bitstream_destroy(generator);

    str = "1101011011";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    str = "10011";
    generator = bitstream_create(strlen(str));
    bitstream_read_from_string(generator, str);
    frame = crc_encode(input, generator);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "11010110111110"));
    crc_destroy(frame);
    bitstream_destroy(input);
    bitstream_destroy(generator);

    str = "1101011011011100";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    str = "1101110111011101";
    generator = bitstream_create(strlen(str));
    bitstream_read_from_string(generator, str);
    frame = crc_encode(input, generator);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "1101011011011100110101101101110"));
    crc_destroy(frame);
    bitstream_destroy(input);
    bitstream_destroy(generator);

    printf("    => CRC tests passed!\n");
}
