# SkyCrow
# Overo Module
# Makefile
SHELL = /bin/sh

# C++ compiler
CXX = g++

# OpenCV library path
OPENCV_PATH = ./deps/opencv/lib/

# OpenCV libraries
OPENCV_LIBS = $(addprefix $(OPENCV_PATH),libopencv_core.so libopencv_imgcodecs.so libopencv_videoio.so)

SOURCE_PATH = ./source/

BUILD_PATH = ./build/

# Main build
overo: setup $(BUILD_PATH)overo.o $(BUILD_PATH)core.o 
	$(CXX) $(BUILD_PATH)overo.o $(BUILD_PATH)core.o $(OPENCV_LIBS) -o overo

# Unit testing build
test: setup $(BUILD_PATH)test.o $(BUILD_PATH)tcore.o
	$(CXX) $(BUILD_PATH)test.o $(BUILD_PATH)tcore.o $(OPENCV_LIBS) -o test

# Build core.cpp for unit testing
$(BUILD_PATH)tcore.o: $(SOURCE_PATH)core.cpp
	$(CXX) -c -D TEST=true $(SOURCE_PATH)core.cpp -o tcore.o

# Generic build rule for any object file
$(BUILD_PATH)%.o: $(SOURCE_PATH)%.c
	$(CXX) -c $< -o $@

# Create/clear build directory
setup:
	@if [[ -d $(BUILD_PATH) ]]; then rm -rf $(BUILD_PATH)*; else mkdir $(BUILD_PATH); fi

# Remove build directory
clean:
	@rm -rf $(BUILD_PATH)