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

/// Creates a new bitstream as a copy of the given bitstream
BitStream *bitstream_copy(const BitStream *stream, size_t length) {
    BitStream *new_stream;
    size_t nbytes, stream_bytes;

    new_stream = (BitStream*)malloc(sizeof(BitStream));

    new_stream->length = length;
    nbytes = (int)ceil(new_stream->length/8.0);
    stream_bytes = (int)ceil(stream->length/8.0);
    new_stream->bytes = (unsigned char*)malloc(nbytes);
    memcpy(new_stream->bytes, stream->bytes, (int)fmin(nbytes, stream_bytes));

    return new_stream;
}

BitStream *bitstream_concat(const BitStream *lhs, const BitStream *rhs) {
    BitStream *new_stream;
    size_t nbytes, stream_bytes, i;

    new_stream = (BitStream*)malloc(sizeof(BitStream));

    new_stream->length = lhs->length + rhs->length;
    nbytes = (int)ceil(new_stream->length/8.0);
    new_stream->bytes = (unsigned char*)malloc(nbytes);

    stream_bytes = (int)ceil(lhs->length/8.0);
    memcpy(new_stream->bytes, lhs->bytes, stream_bytes);

    for (i = lhs->length; i < new_stream->length; i++) {
        bitstream_set(new_stream, i, bitstream_get(rhs, i - lhs->length));
    }

    return new_stream;
}

void bitstream_destroy(BitStream *stream) {
    free(stream->bytes);
    free(stream);
}

unsigned char inline bitstream_get(const BitStream *stream, size_t bit) {
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

/// Calculates if the stream lhs represents a smaller binary number than rhs
int bitstream_lt(const BitStream *lhs, const BitStream *rhs) {
    size_t lhs_bytes, rhs_bytes, i;

    // Calculate the bytes in the streams
    lhs_bytes = (size_t)ceil(lhs->length/8.0);
    rhs_bytes = (size_t)ceil(rhs->length/8.0);

    // let i be the maximum byte index of lhs or rhs
    i = (size_t)fmax(lhs_bytes, rhs_bytes) - 1;

    // Loop through any extra bytes in rhs
    while (i >= lhs_bytes) {
        if (rhs->bytes[i]) {
            // If any extra bytes from rhs are not 0, it is larger than lhs
            return 1;
        }
        i -= 1;
    }

    // Loop through any extra bytes in lhs
    while (i >= rhs_bytes) {
        if (lhs->bytes[i]) {
            // If any extra bytes from lhs are not 0, it is larger than rhs
            return 0;
        }
        i -= 1;
    }

    // Loop through bytes in both lhs and rhs
    while (i >= 0) {
        if (lhs->bytes[i] < rhs->bytes[i]) {
            // If any byte of lhs is less than that byte in rhs, lhs is less than rhs (they must be equal up until this point)
            return 1;
        } else if (lhs->bytes[i] > rhs->bytes[i]) {
            // If any byte of lhs is greater than that byte in rhs, lhs is greater than rhs (they must be equal up until this point)
            return 0;
        }

        // Otherwise the bytes are equal, so continue
        i -= 1;
    }

    // The streams are equal
    return 0;
}

void bitstream_sll(BitStream *stream) {
    size_t nbytes, i;
    int carry, tmp;

    nbytes = (size_t)ceil(stream->length/8.0);

    carry = 0;
    for (i = 0; i < nbytes; i++) {
        tmp = bitstream_get(stream, (i*8) + 7);
        stream->bytes[i] <<= 1;
        bitstream_set(stream, i*8, carry);
        carry = tmp;
    }

    // If the last byte is not a full byte, we need to reset the overflow bit to 0 (for future calculations)
    if (stream->length % 8) {
        bitstream_set(stream, stream->length, 0);
    }
}

void bitstream_srl(BitStream *stream) {
    int nbytes, i, carry, tmp;

    nbytes = (size_t)ceil(stream->length/8.0);

    carry = 0;
    for (i = nbytes - 1; i >= 0; i--) {
        tmp = bitstream_get(stream, i*8);
        stream->bytes[i] >>= 1;
        bitstream_set(stream, (i*8) + 7, carry);
        carry = tmp;
    }
}

/// Performs a bitwise xor on lhs, given the rhs argument
void bitstream_xor(BitStream *lhs, const BitStream *rhs) {
    int nbytes, i;

    nbytes = (size_t)fmin(ceil(lhs->length/8.0), ceil(rhs->length/8.0));

    for (i = 0; i < nbytes; i++) {
        lhs->bytes[i] ^= rhs->bytes[i];
    }
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
    BitStream *stream, *stream2, *stream3;

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

    stream2 = bitstream_copy(stream, stream->length);
    bitstream_destroy(stream);

    assert(!strcmp(b, "110100110101"));

    bitstream_sll(stream2);
    bitstream_write_to_string(stream2, b);
    assert(!strcmp(b, "011010011010"));

    bitstream_srl(stream2);
    bitstream_write_to_string(stream2, b);
    assert(!strcmp(b, "110100110100"));

    a = "1101";
    stream = bitstream_create(strlen(a));
    bitstream_read_from_string(stream, a);
    bitstream_xor(stream2, stream);
    bitstream_write_to_string(stream2, b);
    assert(!strcmp(b, "000000110100"));

    stream3 = bitstream_concat(stream, stream2);
    bitstream_write_to_string(stream3, b);
    assert(!strcmp(b, "1101000000110100"));

    bitstream_destroy(stream);
    bitstream_destroy(stream2);
    bitstream_destroy(stream3);

    printf("    => Bitstream tests passed!\n");
}
