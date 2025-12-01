# Compiler
CXX = g++

# Flags: -std=c++17, -Wall (all warnings), -O2 (optimize), -g (debug symbols)
CXXFLAGS = -std=c++17 -Wall -O2 -g

# Executable name
TARGET = recommender

# Source files
SRCS = main.cpp SocialNetwork.cpp $(wildcard algorithms/*.cpp)

# Object files (derived from SRCS)
OBJS = $(SRCS:.cpp=.o)

# Default target (what 'make' does)
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .cpp to .o (object) files
# $< is the .cpp file, $@ is the .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJS)

# A simple rule to run your test harness
