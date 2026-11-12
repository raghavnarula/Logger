# Define the compiler and optimization/warning flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

# Define the final executable name
TARGET = logger

# Define the source files and corresponding object files
SRCS = main.cpp LogGenerator.cpp Block.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target when you just run 'make'
all: $(TARGET)

# Link the object files together to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Generic rule to compile any .cpp file into a .o object file
%.o: %.cpp LogGenerator.h Libraries.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts and log files
clean:
	rm -f $(OBJS) $(TARGET) /mnt/sda1/app.log