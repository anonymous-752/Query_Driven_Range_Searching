# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -fopenmp -march=native -O3 -pedantic -I$(CONDA_PREFIX)/include/eigen3


# Include directories
INCLUDES = -I.

# Source files
SRCS = file.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = file

# Default target
all: $(EXEC)

# Linking
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)

# Phony targets
.PHONY: all clean

