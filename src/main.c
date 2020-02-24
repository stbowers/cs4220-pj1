#include <stdio.h>
#include <string.h>
#include <bitstream.h>
#include <crc.h>
#include <hamming.h>

/// Entry point for part 1.1
void part_1_1(char *input_str, int quiet) {
    BitStream *input;
    HammingFrame *frame;
    char *output;

    if (!quiet) {
        printf("===== Part 1.1 =====\n");
        printf("Input: %s\n", input_str);
    }
    
    input = bitstream_create(strlen(input_str));
    bitstream_read_from_string(input, input_str);
    frame = hamming_encode(input);
    output = (char*)malloc(frame->frame_stream->length + 1);
    bitstream_write_to_string(frame->frame_stream, output);

    if (quiet) {
        printf("%s\n", output);
    } else {
        printf("Output: %s\n", output);
    }

    free(output);
    hamming_destroy(frame);
    bitstream_destroy(input);
}

/// Entry point for part 1.2
void part_1_2(char *input_str, int quiet) {
    BitStream *input, *output;
    HammingFrame *frame;
    char *output_str;

    if (!quiet) {
        printf("===== Part 1.2 =====\n");
        printf("Input: %s\n", input_str);
    }
    
    input = bitstream_create(strlen(input_str));
    bitstream_read_from_string(input, input_str);
    frame = hamming_frame_from_stream(input);
    hamming_fix_errors(frame);
    output = hamming_decode(frame);
    output_str = (char*)malloc(output->length + 1);
    bitstream_write_to_string(output, output_str);

    if (quiet) {
        printf("%s\n", output_str);
    } else {
        printf("Output: %s\n", output_str);
    }

    free(output_str);
    bitstream_destroy(output);
    hamming_destroy(frame);
}

/// Entry point for part 2
void part_2(char *input_str, char *generator_str, int quiet) {
    BitStream *input, *generator;
    CRCFrame *frame;
    char *output;

    if (!quiet) {
        printf("===== Part 2 =====\n");
        printf("Input: %s\n", input_str);
    }
    
    input = bitstream_create(strlen(input_str));
    bitstream_read_from_string(input, input_str);

    generator = bitstream_create(strlen(generator_str));
    bitstream_read_from_string(generator, generator_str);

    frame = crc_encode(input, generator);

    output = (char*)malloc(frame->frame_bits + 1);
    bitstream_write_to_string(frame->frame_stream, output);

    if (quiet) {
        printf("%s\n", output);
    } else {
        printf("Output: %s\n", output);
    }

    free(output);
    crc_destroy(frame);
    bitstream_destroy(input);
    bitstream_destroy(generator);
}

/// Prints information about how to use the program
void print_usage() {
    printf("Usage:\n");
    printf("pj1 --part={part} --input={input} [--generator={generator}] [--test] [--quiet]\n");
    printf("\n");
    printf("Where:\n");
    printf("       {part}: The part to run (1.1, 1.2, or 2)\n");
    printf("      {input}: The input to use\n");
    printf("  {generator}: The generator to use for part 2 (required for part 2, ignored otherwise)\n");
    printf("\n");
    printf("--test and --quiet are optional, and do the following:\n");
    printf("   --test: Runs tests\n");
    printf("  --quiet: Supresses any output other than the final output of the program\n");
}

/// Runs a series of tests for the code
void run_tests() {
    printf("===== Running tests =====\n");

    bitstream_test();
    crc_test();
    hamming_test();

    printf("Tests passed!\n");
}

int main(int argc, char **argv) {
    int i, test, quiet;
    char *arg;

    char *part = NULL;
    char *input = NULL;
    char *generator = NULL;

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
        } else if (!strncmp("--generator=", arg, 12)) {
            // If this argument starts with "--generator=" set generator
            generator = &arg[12];
        } else if (!strcmp("--test", arg)) {
            // If this argument is --test, go in to test mode
            test = 1;
        } else if (!strcmp("--quiet", arg)) {
            // If this argument is --quiet, go in to quiet mode
            quiet = 1;
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
        // Run part 1.1
        part_1_1(input, quiet);
    } else if (!strcmp("1.2", part)) {
        // Run part 1.2
        part_1_2(input, quiet);
    } else if (!strcmp("2", part)) {
        // Check for generator for part 2
        if (!generator) {
            print_usage();
            exit(1);
        }

        // Run part 2
        part_2(input, generator, quiet);
    } else {
        // If the part given was not valid print usage and exit
        print_usage();
        exit(1);
    }

    exit(0);
}

