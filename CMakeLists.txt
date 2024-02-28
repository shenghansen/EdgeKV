cmake_minimum_required(VERSION 3.22)
project( EdgeKV )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party/spdlog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-march=native ${CMAKE_CXX_FLAGS}")

#宏定义
add_definitions(-DHIGH_PERFORMANCE)

include_directories(include)

set(DATACENTER_SRCS
  ${CMAKE_SOURCE_DIR}/src/datacenterKV.cpp
  ${CMAKE_SOURCE_DIR}/src/utill/leveldb.cpp
)

add_executable(datacenterKV  ${DATACENTER_SRCS})
add_executable(edgeKV src/edgeKV.cpp)

target_link_libraries(edgeKV spdlog leveldb snappy)
target_link_libraries(datacenterKV spdlog leveldb snappy)