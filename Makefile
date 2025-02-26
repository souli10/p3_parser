# Makefile for bottom-up parser (Practice 3)
# Group 

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source files and objects
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Executable name
TARGET = parser

# Main rules
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) *_p3dbg.txt

run: $(TARGET)
	./$(TARGET) $(if $(INPUT),$(INPUT),tests/test_input1.cscn)

test: $(TARGET)
	./$(TARGET) tests/test_input1.cscn

# Additional targets
help:
	@echo "Usage:"
	@echo "  make          - Build the parser"
	@echo "  make clean    - Remove compiled files"
	@echo "  make test     - Run tests with sample input"
	@echo "  make run INPUT=<file> - Run parser with custom input file"

.PHONY: all clean run test help