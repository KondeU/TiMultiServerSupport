cmake_minimum_required(VERSION 3.10)
project(MSS_TEST)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../include)

include_directories(../build/dependency/cereal/include)
include_directories(../build/dependency/zeromq/zeromq/include)
include_directories(../build/dependency/zeromq/cppzmq/include)

link_directories(../build/dependency/zeromq/zeromq/lib)
link_libraries(libzmq)

file(GLOB_RECURSE INC ../include/*)

add_executable(TestRPC_Client TestRPC_Client.cpp ${INC})
add_executable(TestRPC_Server TestRPC_Server.cpp ${INC})
add_executable(TestStatusSync_Client TestStatusSync_Client.cpp ${INC})
add_executable(TestStatusSync_Server TestStatusSync_Server.cpp ${INC})
