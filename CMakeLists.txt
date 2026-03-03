cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(Rynox-Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(DEPENDENCIES_OWN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Own)
set(DEPENDENCIES_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Third_Party)

# Dependecies Third_Party
add_subdirectory(${DEPENDENCIES_THIRD_PARTY_DIR}/glad)

# Dependencies Own
add_subdirectory(${DEPENDENCIES_OWN_DIR}/RynoxCommon)
add_subdirectory(${DEPENDENCIES_OWN_DIR}/RynoxMath)
add_subdirectory(${DEPENDENCIES_OWN_DIR}/RynoxRenderer)
add_subdirectory(${DEPENDENCIES_OWN_DIR}/RynoxECS)

# Root
add_subdirectory(RynoxCore)
add_subdirectory(RynoxEditor)