#------------------------------------------------------------------------------
# Usage:
#   make        # Build the car executable
#   make clean  # Remove compiled files
#   make run    # Run the executable
#------------------------------------------------------------------------------

# Default to g++ if CXX is not set in the environment
CXX ?= g++

# Detect whether opencv4 is installed
CHECK_OPENCV4 := $(shell pkg-config --list-all | grep -q opencv4 && echo 1 || echo 0)

ifeq ($(CHECK_OPENCV4), 1)
  # Retrieve both include flags and library linking flags for OpenCV4
  OPENCV_CFLAGS := $(shell pkg-config --cflags opencv4)
  OPENCV_LIBS   := $(shell pkg-config --libs opencv4)
else
  # Fall back to legacy OpenCV if opencv4 not found
  OPENCV_CFLAGS := $(shell pkg-config --cflags opencv)
  OPENCV_LIBS   := $(shell pkg-config --libs opencv)
endif

# Directories
BIN_DIR   := bin
SRC_DIR   := src
INC_DIR   := include
OBJ_DIR   := obj

# Collect all .cpp files in src/
SOURCES   := $(wildcard $(SRC_DIR)/*.cpp)
# Convert each .cpp into a .o file in obj/
OBJECTS   := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Final executable name
TARGET    := $(BIN_DIR)/car

# Compiler flags
# Append OPENCV_CFLAGS so compiler can find OpenCV headers
CXXFLAGS  := -std=c++17 -O2 -I$(INC_DIR) $(OPENCV_CFLAGS)

# Additional libraries we need (Vitis AI, pthread, etc.)
# We'll append OPENCV_LIBS so linker can find OpenCV libraries
LDLIBS    := \
    -lvitis_ai_library-ultrafast \
    -lvitis_ai_library-dpu_task \
    -lvitis_ai_library-xnnpp \
    -lvitis_ai_library-model_config \
    -lvitis_ai_library-math \
    -lvart-util \
    -lxir \
    -pthread \
    -ljson-c \
    -lglog \
    $(OPENCV_LIBS)

# Default rule: Build the final executable
all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

# Compile each .cpp into a .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure necessary directories exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Clean rule: Remove compiled files
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Run the program
run: all
	@echo "Running $(TARGET) ..."
	@./$(TARGET)
