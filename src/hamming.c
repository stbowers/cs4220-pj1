#include <hamming.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/// Given a message length (in bits), msg_len, returns the number of bits required for the hamming encoding of
/// that message
size_t hamming_frame_length(size_t message_length) {
    // Let m be the message length, and h be the hamming length.
    // They are related by:
    //
    // h = 2^r - 1
    // m = 2^r - r - 1
    //
    // where r is the number of parity bits, and is >= 2
    
    size_t h, m, r;

    // Find a suitable value for r, starting at 2
    for (r = 2; (m = (size_t)pow(2, r) - r - 1) < message_length; r++) {}
    //           ^                              ^- Keep looping as long as m is less than msg_len
    //           |- Calculate the number of data bits we can cover with r parity bits

    // Calculate the hamming length h
    h = (size_t)pow(2, r) - 1;

    // Return h minus any extra bits we don't need
    return h - (m - message_length);
}

/// Given the length of a hamming code (in bits), hamming_len, returs the number of message bits in the code
size_t hamming_message_length(size_t frame_length) {
    // Let m be the message length, and h be the hamming length.
    // They are related by:
    //
    // h = 2^r - 1
    // m = 2^r - r - 1
    //
    // where r is the number of parity bits, and is >= 2
    //
    // Therefore we can use h to solve for r:
    // r = log2(h + 1)
    //
    // In order to account for hamming codes that use less bits than they have capacity for, r should be rounded up

    size_t h, m, r;

    // Calculate r - the number of parity bits
    r = (size_t)ceil(log2(frame_length + 1));

    // Calculate m - the number of bits that can be covered by r parity bits
    m = (size_t)pow(2, r) - r - 1;

    // Calculate h - the number of bits in a full hamming code with r parity bits
    h = (size_t)pow(2, r) - 1;

    // Return the number of bits that can be covered minus the number of bits left out
    return m - (h - frame_length);
}

/// Encodes the given bitstream into a new hamming frame
HammingFrame* hamming_encode(BitStream *input) {
    size_t input_bit, frame_bit, i;

    // Create a new frame
    HammingFrame *frame = (HammingFrame*)malloc(sizeof(HammingFrame));

    // Calculate the bits in the frame
    frame->message_bits = input->length;
    frame->frame_bits = hamming_frame_length(input->length);

    // Create the frame bit stream
    frame->frame_stream = bitstream_create(frame->frame_bits);

    // Loop through each bit of the frame, calculating it's value
    input_bit = 0;
    for (frame_bit = 0; frame_bit < frame->frame_bits; frame_bit++) {
        // Loop through all parity bits up until this bit
        for (i = 1; i < (frame_bit + 1); i = i << 1) {
            if ((frame_bit + 1) & i) {
                bitstream_sum(frame->frame_stream, i - 1, bitstream_get(input, input_bit));
            }
        }

        // If i == output_bit + 1 this is a parity bit (and we don't have to do anything)
        // Otherwise it's a data bit, and we should write it to the output and increment the input_bit counter
        if (i != frame_bit + 1) {
            if (bitstream_get(input, input_bit)) {
                bitstream_set(frame->frame_stream, frame_bit, 1);
            }

            input_bit += 1;
        }
    }

    return frame;
}

/// Creates a hamming frame from a bit stream
HammingFrame* hamming_frame_from_stream(BitStream *stream) {
    // Create a new frame
    HammingFrame *frame = (HammingFrame*)malloc(sizeof(HammingFrame));

    // Calculate the bits in the message
    frame->frame_bits = stream->length;
    frame->message_bits = hamming_message_length(stream->length);

    frame->frame_stream = stream;

    return frame;
}

/// Decodes the given hamming frame into a bitstream
/// NOTE! Skips parity bits. User should call hamming_fix_errors first!
BitStream* hamming_decode(HammingFrame *frame) {
    BitStream *output;
    size_t output_bit, frame_bit, i;

    // Create the output bitstream, and start writing to bit 0
    output = bitstream_create(frame->message_bits);
    output_bit = 0;

    for (frame_bit = 0; frame_bit < frame->frame_bits; frame_bit++) {
        // printf("processing input bit %d\n", frame_bit);

        // Loop through all powers of 2 up until the frame bit
        for (i = 1; i < (frame_bit + 1); i = i << 1) {}

        // If i == frame_bit + 1 this is a parity bit, otherwise it's a data bit.
        if (i != frame_bit + 1) {
            bitstream_set(output, output_bit, bitstream_get(frame->frame_stream, frame_bit));
            output_bit++;
        }

        //printf("Output: %d\n", output[0]);
    }

    return output;
}

/// Given a hamming frame, fixes any error found (up to 1 bit of error)
void hamming_fix_errors(HammingFrame *frame) {
    size_t frame_bit, i, error_bit, parity;

    error_bit = 0;

    for (frame_bit = 0; frame_bit < frame->frame_bits; frame_bit++) {
        // printf("processing input bit %d/%d (= %d)\n", frame_bit + 1, frame->frame_bits, bitstream_get(frame->frame_stream, frame_bit));

        // Loop through all powers of 2 up until the frame bit
        for (i = 1; i < (frame_bit + 1); i = i << 1) {}

        // If i == frame_bit + 1 this is a parity bit, otherwise it's a data bit.
        if (i == frame_bit + 1) {
            // Parity bit - check for error
            parity = 0;
            for (i = 0; i < frame->frame_bits + 1; i++) {
                if (!(i & (frame_bit + 1))) {
                    continue;
                }
                parity ^= bitstream_get(frame->frame_stream, i - 1);
                // printf("Checking %d (%d => %d)\n", i, bitstream_get(frame->frame_stream, i - 1), parity);
            }

            if (parity) {
                // Parity was not 0, so there is an error with this parity bit
                error_bit += frame_bit + 1;
                // printf("Parity error on bit %d!\n", frame_bit + 1);
            }
        }

        //printf("Output: %d\n", output[0]);
    }

    if (error_bit) {
        //printf("Detected error on bit %d...\n", error_bit);

        bitstream_toggle(frame->frame_stream, error_bit - 1);
    }
}

/// Free memory allocated for a hamming frame
void hamming_destroy(HammingFrame *frame) {
    bitstream_destroy(frame->frame_stream);
    free(frame);
}

void hamming_test() {
    char *str, output[75];
    BitStream *input, *output_stream;
    HammingFrame *frame;

    printf("  => Testing hamming functions\n");

    assert(hamming_frame_length(1) == 3);
    assert(hamming_frame_length(2) == 5);
    assert(hamming_frame_length(3) == 6);
    assert(hamming_frame_length(4) == 7);
    assert(hamming_frame_length(5) == 9);
    assert(hamming_frame_length(6) == 10);
    assert(hamming_frame_length(7) == 11);
    assert(hamming_frame_length(8) == 12);
    assert(hamming_frame_length(9) == 13);
    assert(hamming_frame_length(10) == 14);
    assert(hamming_frame_length(11) == 15);

    assert(hamming_message_length(15) == 11);
    assert(hamming_message_length(14) == 10);
    assert(hamming_message_length(13) == 9);
    assert(hamming_message_length(12) == 8);
    assert(hamming_message_length(11) == 7);
    assert(hamming_message_length(10) == 6);
    assert(hamming_message_length(9) == 5);
    assert(hamming_message_length(7) == 4);
    assert(hamming_message_length(6) == 3);
    assert(hamming_message_length(5) == 2);
    assert(hamming_message_length(3) == 1);

    str = "1";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    frame = hamming_encode(input);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "111"));
    hamming_destroy(frame);
    bitstream_destroy(input);

    str = "10010000";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    frame = hamming_encode(input);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "001100100000"));
    hamming_destroy(frame);
    bitstream_destroy(input);
    
    str = "1101001100110101";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    frame = hamming_encode(input);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "011110110011001110101"));
    hamming_destroy(frame);
    bitstream_destroy(input);

    str = "101";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    frame = hamming_frame_from_stream(input);
    hamming_fix_errors(frame);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "111"));
    output_stream = hamming_decode(frame);
    bitstream_write_to_string(output_stream, output);
    assert(!strcmp(output, "1"));
    bitstream_destroy(output_stream);
    hamming_destroy(frame);

    str = "00110010001";
    input = bitstream_create(strlen(str));
    bitstream_read_from_string(input, str);
    frame = hamming_frame_from_stream(input);
    hamming_fix_errors(frame);
    bitstream_write_to_string(frame->frame_stream, output);
    assert(!strcmp(output, "00110010000"));
    output_stream = hamming_decode(frame);
    bitstream_write_to_string(output_stream, output);
    assert(!strcmp(output, "1001000"));
    bitstream_destroy(output_stream);
    hamming_destroy(frame);

    printf("    => Hamming tests passed!\n");
}
