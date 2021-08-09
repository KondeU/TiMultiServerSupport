cmake_minimum_required(VERSION 3.10)
project(TestBroadcastRpc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../../include)

include_directories(../../build/dependency/cereal/include)
include_directories(../../build/dependency/zeromq/zeromq/include)
include_directories(../../build/dependency/zeromq/cppzmq/include)

link_directories(../../build/dependency/zeromq/zeromq/lib)
if(WIN32)
  link_libraries(libzmq)
else()
  link_libraries(pthread zmq)
endif()

file(GLOB_RECURSE INC ../../include/*)

add_executable(TestBroadcastRpcClient TestBroadcastRpc.cpp ${INC})
add_executable(TestBroadcastRpcServer TestBroadcastRpc.cpp ${INC})
target_compile_definitions(TestBroadcastRpcServer PRIVATE SERVER=)
