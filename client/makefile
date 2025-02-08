# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror
BUILD_CFLAGS = -O2 $(CFLAGS)  # Optimization flags

# Directories
SRC_DIR = src
OBJ_DIR = out
BUILD_DIR = build

# Source and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
BUILD_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# Target executable name
EXECUTABLE_NAME = battleship-client
TARGET = $(OBJ_DIR)/$(EXECUTABLE_NAME)
BUILD_TARGET = $(BUILD_DIR)/$(EXECUTABLE_NAME)

# Rule to create the output directories
$(OBJ_DIR) $(BUILD_DIR):
	@mkdir -p $@

# Rule to build the target executable
$(TARGET): $(OBJ)
	@$(CC) -o $@ $^ $(CFLAGS)

# Rule to build the optimized target executable
$(BUILD_TARGET): $(BUILD_OBJ)
	@$(CC) -o $@ $^ $(BUILD_CFLAGS)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) -c -o $@ $< $(CFLAGS)

# Rule to compile source files into optimized object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) -c -o $@ $< $(BUILD_CFLAGS)

# Phony targets
.PHONY: all clean test install uninstall format build

# Default target
all: $(TARGET)

# Build optimized target
build: $(BUILD_TARGET)

# Test target
test: $(TARGET)
	@./$(TARGET) 127.0.0.1 50000

# Clean target
clean:
	@rm -rf $(OBJ_DIR)/*.o $(BUILD_DIR)/*.o $(TARGET) $(BUILD_TARGET)

# Install target (example)
install: $(BUILD_TARGET)
	@cp $(BUILD_TARGET) /usr/local/bin/$(EXECUTABLE_NAME)

# Uninstall target (example)
uninstall:
	@rm -f /usr/local/bin/$(EXECUTABLE_NAME)

# Format the code using clang-format and the LLVM coding style
format:
	find $(SRC_DIR) -name '*.c' -o -name '*.h' | xargs clang-format -style=LLVM -i
