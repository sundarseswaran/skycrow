# SkyCrow
# Overo Module
# Makefile
SHELL = /bin/bash

# C++ compiler
CXX = g++

# OpenCV library path
OPENCV_PATH = deps/opencv/lib/

# OpenCV libraries
OPENCV_LIBS = $(addprefix $(OPENCV_PATH),libopencv_core.so libopencv_imgcodecs.so libopencv_videoio.so)

SOURCE_PATH = source/

BUILD_PATH = build/

all: overo

# Main build
overo: setup $(BUILD_PATH)overo.o $(BUILD_PATH)core.o $(BUILD_PATH)sqlite3.o
	$(CXX) $(BUILD_PATH)overo.o $(BUILD_PATH)core.o $(BUILD_PATH)sqlite3.o $(OPENCV_LIBS) -o overo

# Unit testing build
test: setup $(BUILD_PATH)test.o $(BUILD_PATH)tcore.o $(BUILD_PATH)sqlite3.o
	$(CXX) $(BUILD_PATH)test.o $(BUILD_PATH)tcore.o $(BUILD_PATH)sqlite3.o $(OPENCV_LIBS) -o test

$(BUILD_PATH)core.o: $(SOURCE_PATH)core.cpp
	$(CXX) -c $(SOURCE_PATH)core.cpp -o $(BUILD_PATH)core.o

# Build core.cpp for unit testing
$(BUILD_PATH)tcore.o: $(SOURCE_PATH)core.cpp
	$(CXX) -c -D TEST=true $(SOURCE_PATH)core.cpp -o $(BUILD_PATH)tcore.o

# Build SQLite
$(BUILD_PATH)sqlite3.o: deps/sqlite/sqlite3.c
	$(CXX) -c $< -o $(BUILD_PATH)sqlite3.o

# Generic build rule for any object file
$(BUILD_PATH)%.o: $(SOURCE_PATH)%.cpp
	$(CXX) -c $< -o $@

# Create/clear build directory
setup:
	@if [[ -d $(BUILD_PATH) ]]; then rm -rf $(BUILD_PATH)*; else mkdir $(BUILD_PATH); fi

# Remove build directory
clean:
	@rm -rf $(BUILD_PATH)