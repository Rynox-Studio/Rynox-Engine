cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(RynoxRenderer-OpenGL LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*
)

add_library(${PROJECT_NAME}
    SHARED
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/RynoxCore/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        glad
        RynoxCommon
        RynoxMath
    PRIVATE
        opengl32
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RNX_RENDERER_EXPORTS)
endif()

set_target_properties(${PROJECT_NAME}
PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/modules"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PROJECT_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PROJECT_NAME}"

    OUTPUT_NAME "${PROJECT_NAME}-v${PROJECT_VERSION}-$<CONFIG>-$<PLATFORM_ID>"
)