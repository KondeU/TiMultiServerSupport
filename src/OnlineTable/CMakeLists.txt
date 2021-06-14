cmake_minimum_required(VERSION 3.10)
project(online_table)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# === NB!!! Import the TiRPC!!! ===
include(3rd/TiRPC/msvc_x64/TiRPC.cmake)

option(USE_CONSOLE "Showing console" OFF)
if(NOT USE_CONSOLE)
  if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /entry:mainCRTStartup")
  else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
  endif()
endif()

file(GLOB_RECURSE GUI src/*.ui)
file(GLOB_RECURSE RES src/*.qrc)
file(GLOB_RECURSE SRC src/*.cpp src/*.h)

add_executable(online_table ${GUI} ${RES} ${SRC})
target_link_libraries(online_table Qt5::Core Qt5::Gui Qt5::Widgets)

# NB: Precompile the TiRPC headers, optional.
PrecompileTiRPC(online_table)
