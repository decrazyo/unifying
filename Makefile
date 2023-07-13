
CC = gcc
CFLAGS = -Wall
LDFLAGS =

NAME = main
SRC = src/
BIN = bin/

TARGET := $(BIN)$(NAME)
SOURCES := $(wildcard $(SRC)*.c)
OBJECTS := $(SOURCES:$(SRC)%.c=$(BIN)%.o)

.PHONY: all
all: $(BIN) $(TARGET)

.PHONY: docs
docs:
	doxygen

.PHONY: clean
clean:
	rm -f $(BIN)*

$(BIN):
	mkdir -p bin

$(BIN)%.o: $(SRC)%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
