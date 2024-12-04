cmake_minimum_required(VERSION 3.10)
project(GPIO74HC259)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含 include 目錄
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加源文件
add_executable(gpio74hc259_test test/main.cpp src/GPIO74HC259.cpp)
