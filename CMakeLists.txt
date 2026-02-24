cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(Rynox-Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(DEPENDECIES_OWN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependecies/Own)
set(DEPENDECIES_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependecies/Third-Party)

# Dependecies
add_subdirectory(${DEPENDECIES_OWN_DIR}/RynoxMath)
add_subdirectory(${DEPENDECIES_OWN_DIR}/RynoxRenderer)

# Root
add_subdirectory(RynoxCore)
add_subdirectory(RynoxEditor)