cmake_minimum_required(VERSION 3.22)
project( EdgeKV )
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party/spdlog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-march=native -g ${CMAKE_CXX_FLAGS}")

#宏定义
# add_definitions(-DHIGH_PERFORMANCE)



include_directories(include)
include_directories(/mnt/nvme0/home/shs/brpc/output/include)
link_directories(/mnt/nvme0/home/shs/brpc/output/lib)

set(DATACENTER_SRCS
  ${CMAKE_SOURCE_DIR}/src/datacenterKV.cpp
  ${CMAKE_SOURCE_DIR}/src/utill/leveldb.cpp
  ${CMAKE_SOURCE_DIR}/src/utill/KVService.pb.cc
  ${CMAKE_SOURCE_DIR}/src/datacenter/server.cpp
)

set(EDGE_SRCS
  ${CMAKE_SOURCE_DIR}/src/edgeKV.cpp
  ${CMAKE_SOURCE_DIR}/src/utill/leveldb.cpp
  ${CMAKE_SOURCE_DIR}/src/utill/KVService.pb.cc
  ${CMAKE_SOURCE_DIR}/src/edge/client.cpp
${CMAKE_SOURCE_DIR}/src/edge/EdgeService.cpp
${CMAKE_SOURCE_DIR}/src/edge/S3FIFO.cpp

)

add_executable(datacenterKV  ${DATACENTER_SRCS})
add_executable(edgeKV ${EDGE_SRCS})
add_executable(testleveldb test/test_leveldb.cpp src/utill/leveldb.cpp)


target_link_libraries(edgeKV spdlog leveldb snappy brpc protobuf)
target_link_libraries(datacenterKV spdlog leveldb snappy brpc protobuf gflags)
target_link_libraries(testleveldb leveldb fmt)
