# Compiler for C++ files
CXX = g++

# Compiler for C files
CC = gcc

# Compiler flags for C++
CXXFLAGS = -Wall -g 

# Compiler flags for C
CFLAGS = -std=c++11 -Wall -Wextra -Isqlite3 -g 

# Linker flags
LDFLAGS = -lcurl -lssl -lcrypto -lm -lpthread -ldl 

# List of C++ source files
CXXSRC = $(wildcard *.cpp)

# List of C source files
CSRC = $(wildcard sqlite3/*.c)

# Object files generated from C++ source files
CXXOBJ = $(CXXSRC:.cpp=.o)

# Object files generated from C source files
COBJ = $(CSRC:.c=.o)

# Name of the final executable
TARGET = main

# All object files
OBJ = $(CXXOBJ) $(COBJ)

# Define the files to clean
CLEAN_FILES = $(OBJ) $(TARGET).exe

# Detect the operating system and set RM accordingly
ifeq ($(OS), Windows_NT)
    RM = del
    CLEAN_FILES := $(subst /,\,$(CLEAN_FILES))
else
    RM = rm -f
endif

# Default target to build the executable
all: $(TARGET)

# Rule to link all object files into the final executable
$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) 

# Rule to compile C++ source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile C source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
ifeq ($(OS), Windows_NT)
	-$(RM) $(CLEAN_FILES)
else
	$(RM) $(CLEAN_FILES)
endif

# Phony targets
.PHONY: all clean
