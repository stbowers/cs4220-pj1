# Project 1

The goal of this project is to write a program that can encode and decode a hamming message, and encode a crc message.

## Compiling

This project is built with a makefile, which was configured for building on Linux with gcc.
The project does not have any dependencies (besides the c standard library and math library), so it should work on other platforms.

To compile, the four .c files in src/ (bitstream.c, crc.c, hamming.c, and main.c) should be compiled with src/ as an include directory.
This is automatically done with a makefile on Linux systems, and can just be done by running `make` in the root of the project.

## Running

This project can be run directly from the makefile, with `make run ARGS="<args>"` where <args> are the arguments to the program specified below.

The project can also be run directly as a binary after being compiled.
For example, if the project was compiled to ./bin/pj1, it could be run as `./bin/pj1 <args>`

The arguments to the program are listed when the program is run with no arguments.
The essential arguments are:
  - `--part={part}`: specifies which part to run, so for example for part 1.1, the argument would be `--part=1.1`
  - `--input={input}`: The input to the program
  - `--generator={generator}`: The generator to use (only for part 2)
  - `--quiet`: Tells the program to not output any text besides the final output
  - `--test`: Tells the program to run tests

## Examples

```
$ ./bin/pj1 --part=1.1 --input=1101001100110101 --quiet
011110110011001110101
```

```
$ ./bin/pj1 --part=1.2 --input=011110110011001110101
===== Part 1.2 =====
Input: 011110110011001110101
Output: 1101001100110101
```
