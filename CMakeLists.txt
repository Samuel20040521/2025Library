# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(hardware)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directory
set(INCLUDES inc)

# Define external LED library
set(LED_LIBRARY ws2811)

# Add the static library
add_library(hardware STATIC ./src/OFController.cpp ./src/LEDController.cpp)

# Add include directories for the library
target_include_directories(hardware PUBLIC ${INCLUDES})

# Link the external LED library
target_link_libraries(hardware ${LED_LIBRARY})

# Add the main executable for the application
add_executable(AllMain ./src/AllMain.cpp)

# Link the hardware library to the application executable
target_link_libraries(AllMain hardware)

# Add the test executable for testing
add_executable(TestMain ./tests/test_main.cpp)

# Link the hardware library to the test executable
target_link_libraries(TestMain hardware)
