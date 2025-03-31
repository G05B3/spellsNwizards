# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O2
DEBUGFLAGS := -g

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Output executable
TARGET := $(BIN_DIR)/program

# Find all source files and corresponding object files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Debug printing to verify sources and objects
$(info SOURCES: $(SOURCES))
$(info OBJECTS: $(OBJECTS))

# Default target
all: check_sources $(TARGET)

# Check if any source files were found
check_sources:
	@if [ -z "$(SOURCES)" ]; then \
		echo "Error: No source files found in $(SRC_DIR)"; \
		exit 1; \
	fi

# Debug build target
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: all

# Link the final executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories if they don’t exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all debug clean check_sources
