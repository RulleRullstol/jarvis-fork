# Compilers and flags
CXX = clang++
CC = clang
CXXFLAGS = -std=c++23 -Wall -Wextra -pedantic -O2 --debug
CFLAGS = -std=c17 -Wall -Wextra -pedantic -O2
LDFLAGS = -lcurl -ljsoncpp

# Directories
SRC_DIR := .
BUILD_DIR := build
EXEC := mainExec

# Find all .cpp and .c source files
CPP_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
C_SOURCES := $(shell find $(SRC_DIR) -name '*.c')
ALL_SOURCES := $(CPP_SOURCES) $(C_SOURCES)

# Generate corresponding object file paths under build/
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SOURCES))
OBJECTS += $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

# Default target
all: $(EXEC)

# Link all objects to make the executable
$(EXEC): $(OBJECTS)
	@echo Linking $@...
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling C++: $<
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Compiling C: $<
	$(CC) -c $(CFLAGS) $< -o $@

# Clean up build files
clean:
	@echo Cleaning up...
	rm -rf $(BUILD_DIR) $(EXEC)

.PHONY: all clean

