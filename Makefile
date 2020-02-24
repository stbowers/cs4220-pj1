BIN = ./bin
OBJ = ./obj
TARGET = $(BIN)/pj1

$(TARGET): DIRS
	gcc -c -o $(OBJ)/bitstream.o src/bitstream.c -Isrc -Wall
	gcc -c -o $(OBJ)/crc.o src/crc.c -Isrc -Wall
	gcc -c -o $(OBJ)/hamming.o src/hamming.c -Isrc -Wall
	gcc -c -o $(OBJ)/main.o src/main.c -Isrc -Wall
	gcc -o $(TARGET) $(OBJ)/main.o $(OBJ)/bitstream.o $(OBJ)/crc.o $(OBJ)/hamming.o -lm

.PHONY: DIRS
DIRS:
	mkdir -p $(BIN)
	mkdir -p $(OBJ)

test: $(TARGET)
	$(TARGET) --test

run: $(TARGET)
	$(TARGET) $(ARGS)
