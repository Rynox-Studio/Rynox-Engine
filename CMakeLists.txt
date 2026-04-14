cmake_minimum_required(VERSION 3.30)
project(Rynox-Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/MP)
endif()

# Dependencies
add_subdirectory(External)
add_subdirectory(Internal)

# Projects
add_subdirectory(RynoxCore)
add_subdirectory(RynoxEditor)
#add_subdirectory(RynoxRHI)