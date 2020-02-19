#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

typedef struct {
    unsigned char *bytes;
    size_t length;
} BitStream;

BitStream *bitstream_from_string(const char *str);
void bitstream_free(BitStream *stream);

unsigned char bitstream_get(BitStream *stream, size_t bit);
void bitstream_set(BitStream *stream, size_t bit, unsigned char value);
void bitstream_toggle(BitStream *stream, size_t bit);

#endif // __BITSTREAM_H__
