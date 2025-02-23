#------------------------------------------------------------------------------
# Usage:
#   make        # Build all targets (one executable per .cpp in src/)
#   make clean  # Remove all compiled executables
#   make run    # Example rule to run a specific executable
#------------------------------------------------------------------------------

# Default to g++ if CXX is not set in the environment
CXX ?= g++

# Detect whether opencv4 is installed
CHECK_OPENCV4 := $(shell pkg-config --list-all | grep -q opencv4 && echo 1 || echo 0)
ifeq ($(CHECK_OPENCV4), 1)
  OPENCV_FLAGS := $(shell pkg-config --cflags --libs-only-L opencv4)
else
  OPENCV_FLAGS := $(shell pkg-config --cflags --libs-only-L opencv)
endif

# Directories
BIN_DIR   := bin
SRC_DIR   := src
INC_DIR   := include

# Collect all .cpp files in src/
SOURCES   := $(wildcard $(SRC_DIR)/*.cpp)
# Convert each .cpp into an executable in bin/
TARGETS   := $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%,$(SOURCES))

# Compiler flags
CXXFLAGS  := -std=c++17 -O2 -I$(INC_DIR)
# Libraries to link against
LDLIBS    := -lvitis_ai_library-ultrafast \
             -lvitis_ai_library-dpu_task \
             -lvitis_ai_library-xnnpp \
             -lvitis_ai_library-model_config \
             -lvitis_ai_library-math \
             -lvart-util \
             -lxir \
             -pthread \
             -ljson-c \
             -lglog \
             $(OPENCV_FLAGS) \
             -lopencv_core \
             -lopencv_videoio \
             -lopencv_imgproc \
             -lopencv_imgcodecs \
             -lopencv_highgui

# Default rule: build all executables
all: $(TARGETS)

# Build rule for each .cpp -> bin/<executable>
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

# Ensure bin/ directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean rule: remove all generated executables
clean:
	rm -rf $(BIN_DIR)

# Example: If you want a "make run" to run bin/main
run: all
	@echo "Running bin/main ..."
	@./$(BIN_DIR)/main
