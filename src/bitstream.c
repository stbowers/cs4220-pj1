#include <bitstream.h>
#include <math.h>
#include <string.h>

BitStream *bitstream_from_string(const char *str) {
    size_t nbytes, i;

    BitStream *stream = (BitStream*)malloc(sizeof(BitStream));

    stream->length = strlen(str);
    nbytes = (int)ceil(stream->length/8);
    stream->bytes = (unsigned char*)malloc(nbytes);
    memset(stream->bytes, 0, nbytes);

    for (i = 0; i < stream->length; i++) {
        stream->bytes[i/8]
    }

    return stream;
}

void bitstream_free(BitStream *stream);

unsigned char inline bitstream_get(BitStream *stream, size_t bit) {
    return (stream->bytes[bit/8] >> (bit % 8)) & 1;
}

void inline bitstream_set(BitStream *stream, size_t bit, unsigned char value) {
    if (value) {
        stream->bytes[bit/8] |= 1 << (bit % 8);
    } else {
        stream->bytes[bit/8] &= !(1 << (bit % 8));
    }
}

void inline bitstream_set(BitStream *stream, size_t bit) {
    stream->bytes[bit/8] ^= 1 << (bit % 8);
}
