# Compiler and flags
CXX = g++
CXXFLAGS = -g -Wall -Weffc++ -std=c++11 -Iinclude

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

# Output binary
TARGET = $(BIN_DIR)/simulation

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Default target
all: $(TARGET)

# Rule for linking the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for compiling object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all clean