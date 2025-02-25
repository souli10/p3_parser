CC = gcc
CFLAGS = -Wall -Wextra -g

# Directory structure
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
TEST_DIR = tests

# Find all .c files in the source directory
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Create corresponding object file paths
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Target executable
EXECUTABLE = parser

# Default target
all: directories $(EXECUTABLE)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR)

# Link object files to create executable
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE)

# Run the parser with a test file
test: $(EXECUTABLE)
	./$(EXECUTABLE) $(TEST_DIR)/test_input1.cscn

# Run with custom arguments
run: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)

.PHONY: all clean run test directories