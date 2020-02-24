#include <bitstream.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/// Creates a new bitstream filled with 0
BitStream *bitstream_create(size_t len) {
    BitStream *stream;
    size_t nbytes;

    stream = (BitStream*)malloc(sizeof(BitStream));

    stream->length = len;
    nbytes = (int)ceil(stream->length/8.0);
    stream->bytes = (unsigned char*)malloc(nbytes);
    memset(stream->bytes, 0, nbytes);

    return stream;
}

void bitstream_destroy(BitStream *stream) {
    free(stream->bytes);
    free(stream);
}

unsigned char inline bitstream_get(BitStream *stream, size_t bit) {
    return (stream->bytes[bit/8] >> (bit % 8)) & 1;
}

void inline bitstream_set(BitStream *stream, size_t bit, unsigned char value) {
    if (value) {
        stream->bytes[bit/8] |= 1 << (bit % 8);
    } else {
        stream->bytes[bit/8] &= ~(1 << (bit % 8));
    }
}

void inline bitstream_toggle(BitStream *stream, size_t bit) {
    stream->bytes[bit/8] ^= 1 << (bit % 8);
}

void bitstream_sum(BitStream *stream, size_t bit, unsigned char value) {
    stream->bytes[bit/8] ^= value << (bit % 8);
}

void bitstream_read_from_string(BitStream *stream, const char *str) {
    size_t i, bits_to_read;

    bits_to_read = (int)fmin(stream->length, strlen(str));

    for (i = 0; i < bits_to_read; i++) {
        bitstream_set(stream, i, str[i] == '1');
    }
}

void bitstream_write_to_string(BitStream *stream, char *str) {
    size_t i;

    for (i = 0; i < stream->length; i++) {
        if (bitstream_get(stream, i)) {
            str[i] = '1';
        } else {
            str[i] = '0';
        }
    }

    str[stream->length] = 0;
}

/// Tests all bitstream functions
void bitstream_test() {
    printf("  => Testing bitstream functions\n");

    char *a, *b;
    BitStream *stream;

    a = "010101010101";
    b = (char*)malloc(strlen(a) + 1);

    stream = bitstream_create(strlen(a));

    bitstream_read_from_string(stream, a);

    assert(0 == bitstream_get(stream, 0));
    assert(1 == bitstream_get(stream, 1));

    bitstream_set(stream, 5, 0);
    bitstream_set(stream, 6, 1);

    bitstream_toggle(stream, 0);

    bitstream_write_to_string(stream, b);

    bitstream_destroy(stream);

    assert(!strcmp(b, "110100110101"));

    printf("    => Bitstream tests passed!\n");
}
