# Makefile

# Define compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic

# Define targets and dependencies
TARGET = simulation
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Compile and link the program
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile individual object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(TARGET) $(OBJECTS)

# Run target
run: $(TARGET)
	./$(TARGET)

