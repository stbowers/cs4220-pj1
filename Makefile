BIN = ./bin
TARGET = $(BIN)/pj1

$(TARGET): DIRS
	gcc -o $(TARGET) src/main.c -lm

.PHONY: DIRS
DIRS:
	mkdir -p $(BIN)

test: $(TARGET)
	$(TARGET) --test

run: $(TARGET)
	$(TARGET) $(ARGS)
