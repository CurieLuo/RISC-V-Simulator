cmake_minimum_required (VERSION 3.8)
project(main)
set(CMAKE_CXX_STANDARD 17)
include_directories(src include)
set(CMAKE_BUILD_TYPE "release")  # 定义编译类型
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -DDEBUG -g2 -ggdb") # 定义Debug编译参数
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall") # 定义Release编译参数
add_executable(code src/control_unit.cpp src/instruction.cpp src/load_store_buffer.cpp src/reorder_buffer.cpp src/reservation_station.cpp src/main.cpp)

