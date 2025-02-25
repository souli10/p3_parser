# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra -I./include
LDFLAGS = 

# Directorios
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Archivos fuente y objetos
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Nombre del ejecutable
TARGET = parser

# Reglas principales
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) *.exe parsed_*

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET) tests/test_input1.cscn

.PHONY: all clean run test