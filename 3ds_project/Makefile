# Define the toolchain
DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM
CXX = $(DEVKITARM)/bin/arm-none-eabi-g++
CXXFLAGS = -march=armv6k -mtune=mpcore -mfloat-abi=hard -O2 -Wall -std=c++17 -I$(DEVKITPRO)/libctru/include
LDFLAGS = -specs=3dsx.specs -lc -lgcc -lsysbase -L$(DEVKITPRO)/libctru/lib -lctru

# Directories
SRC_DIR = ./src
BUILD_DIR = ./build
OUTPUT_DIR = ./output

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
TARGET = $(OUTPUT_DIR)/3ds_project.3dsx

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(OUTPUT_DIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_DIR)
