CXX = g++
CXXFLAGS = -g -Wall -Weffc++ -std=c++11 -Iinclude

# Source files and object files
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=bin/%.o)

# Default target
all: bin/simulation

# Rule for linking the final executable
bin/simulation: $(OBJS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule for compiling object files
bin/%.o: src/%.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf bin

valgrind:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./bin/simulation config_file.txt
# Phony targets
.PHONY: all clean