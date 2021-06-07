cmake_minimum_required(VERSION 3.10)
project(TestStandardRpc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../../include)

include_directories(../../build/dependency/cereal/include)
include_directories(../../build/dependency/zeromq/zeromq/include)
include_directories(../../build/dependency/zeromq/cppzmq/include)

link_directories(../../build/dependency/zeromq/zeromq/lib)
link_libraries(libzmq)

file(GLOB_RECURSE INC ../../include/*)

add_executable(TestStandardRpcClient TestStandardRpcClient.cpp ${INC})
add_executable(TestStandardRpcServer TestStandardRpcServer.cpp ${INC})
