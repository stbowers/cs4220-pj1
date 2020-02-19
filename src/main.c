#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

/// Given a message length (in bits), msg_len, returns the number of bits required for the hamming encoding of
/// that message
size_t hamming_length(size_t msg_len) {
    // Let m be the message length, and h be the hamming length.
    // They are related by:
    //
    // h = 2^r - 1
    // m = 2^r - r - 1
    //
    // where r is the number of parity bits, and is >= 2
    
    size_t h, m, r;

    // Find a suitable value for r, starting at 2
    for (r = 2; (m = (size_t)pow(2, r) - r - 1) < msg_len; r++) {}
    //           ^                              ^- Keep looping as long as m is less than msg_len
    //           |- Calculate the number of data bits we can cover with r parity bits

    // Calculate the hamming length h
    h = (size_t)pow(2, r) - 1;

    // Return h minus any extra bits we don't need
    return h - (m - msg_len);
}

/// Given the length of a hamming code (in bits), hamming_len, returs the number of message bits in the code
size_t message_length(size_t hamming_len) {
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
    r = (size_t)ceil(log2(hamming_len + 1));

    // Calculate m - the number of bits that can be covered by r parity bits
    m = (size_t)pow(2, r) - r - 1;

    // Calculate h - the number of bits in a full hamming code with r parity bits
    h = (size_t)pow(2, r) - 1;

    // Return the number of bits that can be covered minus the number of bits left out
    return m - (h - hamming_len);
}

/// Given an array of input bytes (big endian order), calculate the hamming code and store it in output
///
/// Output must point to memory with enough allocated space for `hamming_length(m)` bits
void hamming_encode(const char *input, size_t m, char *output) {
    size_t input_bit, output_bit, h, i;

    input_bit = 0;
    h = hamming_length(m);

    // Clear output to 0
    memset(output, 0, (int)ceil(h / 8.0));

    //printf("hamming length = %d\n", h);

    // Loop through each bit of the output, calculating it's value
    for (output_bit = 0; output_bit < h; output_bit++) {
        //printf("processing output bit %d\n", output_bit);

        // Loop through all powers of 2 up until the output bit
        for (i = 1; i < (output_bit + 1); i = i << 1) {
            if ((output_bit + 1) & i) {
                //printf("Setting parity bit %d\n", i);
                //printf("  input bit: %d (value = %d)\n", input_bit, ((input[input_bit/8] >> (input_bit % 8)) & 1));
                output[(i - 1)/8] = output[(i - 1)/8] ^ (((input[input_bit/8] >> (input_bit % 8)) & 1) << ((i - 1) % 8));
            }
        }

        // If i == output_bit + 1 this is a parity bit (and we don't have to do anything)
        // Otherwise it's a data bit, and we should write it to the output and increment the input_bit counter
        if (i != output_bit + 1) {
            //printf("Setting output bit %d\n", output_bit);
            //printf("  input bit: %d (value = %d)\n", input_bit, ((input[input_bit/8] >> (input_bit % 8)) & 1));
            output[output_bit/8] = output[output_bit/8] | (((input[input_bit/8] >> (input_bit % 8)) & 1) << (output_bit % 8));
            input_bit += 1;
        }

        //printf("Output: %d\n", output[0]);
    }
}

/// Given an array of hamming-encoded input bytes (big endian order), retreive the encoded message and
/// store in output. This funciton can correct up to one bit of error.
/// 
/// Output must be an array allocated with enough space for `message_length(h)` bits.
void hamming_decode(const char *input, size_t h, char *output) {
    size_t input_bit, output_bit, m, i, error_bit, parity;

    output_bit = 0;
    error_bit = 0;
    m = message_length(m);

    // Clear output to 0
    memset(output, 0, (int)ceil(h / 8.0));

    for (input_bit = 0; input_bit < h; input_bit++) {
        printf("processing input bit %d\n", input_bit);

        // Loop through all powers of 2 up until the input bit
        for (i = 1; i < (input_bit + 1); i = i << 1) {}

        // If i == input_bit + 1 this is a parity bit, otherwise it's a data bit.
        if (i == input_bit + 1) {
            // Parity bit - check for error
            parity = 0;
            for (i = 0; i < h; i++) {
                if (!(i & (input_bit + 1))) {
                    continue;
                }
                printf("Checking %d\n", i);
                parity ^= (input[i/8] >> (i % 8)) & 1;
            }

            if (parity) {
                // Parity was not 0, so there is an error with this parity bit
                error_bit += input_bit + 1;
                printf("Parity error on bit %d!\n", input_bit + 1);
            }
        }

        //printf("Output: %d\n", output[0]);
    }

    if (error_bit) {
        printf("Detected error on bit %d...", error_bit);
        input[error_bit/8] ^= 1 << (error_bit % 8);
    }

    for (input_bit = 0; input_bit < h; input_bit++) {
        printf("processing input bit %d\n", input_bit);

        // Loop through all powers of 2 up until the input bit
        for (i = 1; i < (input_bit + 1); i = i << 1) {}

        // If i == input_bit + 1 this is a parity bit, otherwise it's a data bit.
        if (i == input_bit + 1) {
            // Parity bit - check for error
            parity = 0;
            for (i = 0; i < h; i++) {
                if (!(i & (input_bit + 1))) {
                    continue;
                }
                printf("Checking %d\n", i);
                parity ^= (input[i/8] >> (i % 8)) & 1;
            }

            if (parity) {
                // Parity was not 0, so there is an error with this parity bit
                error_bit += input_bit + 1;
                printf("Parity error on bit %d!\n", input_bit + 1);
            }
        }

        //printf("Output: %d\n", output[0]);
    }
}

// Reads the input string as a binary number and puts the result in output
// Note: output is a multi-byte number stored in big-endian order
void read_input(char *input, char *output) {
    int i, input_len;

    input_len = strlen(input);

    // Clear output to 0
    memset(output, 0, (int)ceil(input_len / 8.0));

    for (i = 0; i < input_len; i++) {
        if (input[i] == '1') {
            output[i/8] = output[i/8] | (1 << (i % 8));
        }
    }
}

// Converts the bytes in input to a string in output
void write_output(char *input, size_t input_len, char *output) {
    int i;

    for (i = 0; i < input_len; i++) {
        if (input[i/8] & (1 << (i % 8))) {
            output[i] = '1';
        } else {
            output[i] = '0';
        }
    }

    output[input_len] = 0;
}

/// Entry point for part 1.1
void part_1_1(char *input_str) {
    int input_len, output_len;
    char *input_bytes, *output_bytes, *output_str;

    printf("===== Part 1.1 =====\n");
    printf("Input: %s\n", input_str);

    // Allocate space for input and then read input
    input_len = strlen(input_str);
    output_len = hamming_length(input_len);
    input_bytes = (char*)malloc((int)ceil(input_len / 8.0));
    read_input(input_str, input_bytes);

    // Allocate space for output and then encode the input
    output_bytes = (char*)malloc((int)ceil(output_len / 8.0));
    hamming_encode(input_bytes, input_len, output_bytes);

    // Allocate space for the output string and write it out
    output_str = (char*)malloc(output_len + 1);
    write_output(output_bytes, output_len, output_str);

    printf("Output: %s\n", output_str);
}

/// Entry point for part 1.2
void part_1_2(char *input_str) {
    int input_len, output_len;
    char *input_bytes, *output_bytes, *output_str;

    printf("===== Part 1.2 =====\n");
    printf("Input: %s\n", input_str);

    // Calculate the bit lengths for input and output
    input_len = strlen(input_str);
    output_len = message_length(input_len);

    // Allocate space for input and then read input
    input_bytes = (char*)malloc((int)ceil(input_len / 8.0));
    read_input(input_str, input_bytes);

    // Allocate space for output and then encode the input
    output_bytes = (char*)malloc((int)ceil(output_len / 8.0));
    hamming_decode(input_bytes, input_len, output_bytes);

    // Allocate space for the output string and write it out
    output_str = (char*)malloc(output_len + 1);
    write_output(output_bytes, output_len, output_str);

    printf("Output: %s\n", output_str);
}

/// Entry point for part 2
void part_2(char *input) {
    printf("===== Part 2 =====\n");
    printf("Input: %s\n", input);
}

/// Prints information about how to use the program
void print_usage() {
    printf("Usage:\n");
    printf("pj1 --part={part} --input={input}\n");
    printf("\n");
    printf("Where  {part}: The part to run (1.1, 1.2, or 2)\n");
    printf("      {input}: The input to use\n");
}

/// Runs a series of tests on the utility functions
void run_tests() {
    unsigned char input[8], output[8];
    char *input_str, *output_str;

    printf("===== Running tests =====\n");

    assert(hamming_length(1) == 3);
    assert(hamming_length(2) == 5);
    assert(hamming_length(3) == 6);
    assert(hamming_length(4) == 7);
    assert(hamming_length(5) == 9);
    assert(hamming_length(6) == 10);
    assert(hamming_length(7) == 11);
    assert(hamming_length(8) == 12);
    assert(hamming_length(9) == 13);
    assert(hamming_length(10) == 14);
    assert(hamming_length(11) == 15);

    assert(message_length(15) == 11);
    assert(message_length(14) == 10);
    assert(message_length(13) == 9);
    assert(message_length(12) == 8);
    assert(message_length(11) == 7);
    assert(message_length(10) == 6);
    assert(message_length(9) == 5);
    assert(message_length(7) == 4);
    assert(message_length(6) == 3);
    assert(message_length(5) == 2);
    assert(message_length(3) == 1);

    input[0] = 0b1;
    hamming_encode(input, 1, output);
    assert(output[0] == 0b111);

    input[0] = 0b00001001;
    hamming_encode(input, 7, output);
    assert(output[0] == 0b01001100);
    assert(output[1] == 0);

    input[0] = 0b11001011;
    input[1] = 0b10101100;
    hamming_encode(input, 16, output);
    assert(output[0] == 0b11011110);
    assert(output[1] == 0b11001100);
    assert(output[2] == 0b00010101);

    // 0b0101 0101 1010 1010 = 0x55AA
    input_str = "0101010110101010";
    read_input(input_str, input);
    assert(input[0] == 0xAA);
    assert(input[1] == 0x55);
    output_str = (char*)malloc(17);
    write_output(input, 16, output_str);
    assert(!strcmp(input_str, output_str));

    printf("Tests passed!\n");
}

void main(int argc, char **argv) {
    int i, test;
    char *arg;

    char *part = NULL;
    char *input = NULL;

    test = 0;

    // Go through args looking for --part= and --input= to set part and input
    for (i = 1; i < argc; i++) {
        arg = argv[i];

        if (!strncmp("--part=", arg, 7)) {
            // If this argument starts with "--part=" set part
            part = &arg[7];
        } else if (!strncmp("--input=", arg, 8)) {
            // If this argument starts with "--input=" set input
            input = &arg[8];
        } else if (!strcmp("--test", arg)) {
            // If this argument is --test, go in to test mode
            test = 1;
        }
    }

    // If we're in test mode, run tests and exit
    if (test) {
        run_tests();
        exit(0);
    }

    // Both part and input must be specified to run
    if (!part || !input) {
        // If either was not given, print usage and exit
        print_usage();
        exit(1);
    }

    // Determine which part to run
    if (!strcmp("1.1", part)) {
        part_1_1(input);
    } else if (!strcmp("1.2", part)) {
        part_1_2(input);
    } else if (!strcmp("2", part)) {
        part_2(input);
    } else {
        // If the part given was not valid print usage and exit
        print_usage();
        exit(1);
    }

    exit(0);
}

