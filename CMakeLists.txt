cmake_minimum_required(VERSION 3.10)
project(hardware)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(INCLUDES inc)

# 查找 ws2811 庫
find_library(WS2811_LIB ws2811 REQUIRED PATHS /usr/local/lib)

# 添加頭文件路徑
include_directories(${INCLUDES} /usr/local/include/ws2811)

# 添加硬件庫
add_library(hardware STATIC ./src/OFController.cpp ./src/LEDController.cpp)

# 鏈接 ws2811 靜態庫
target_link_libraries(hardware ${WS2811_LIB})

# 添加主程序
add_executable(AllMain ./src/AllMain.cpp)

# 鏈接主程序到硬件庫
target_link_libraries(AllMain hardware)
