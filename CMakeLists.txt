cmake_minimum_required(VERSION 3.10)
project(FCC_OpenGL_Tutorial)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS) # CGLM shows loads of errors without
endif()

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

set(EXE_NAME "MAIN")

include(Examples/CMakeLists.txt)

include_directories(
    ${PROJECT_SOURCE_DIR}/Engine
    ${PROJECT_SOURCE_DIR}/Engine/libs
    ${PROJECT_SOURCE_DIR}/Examples
    ${EXAMPLE_DIRECTORIES}
)

file(GLOB_RECURSE ENGINE_SRC
    "${PROJECT_SOURCE_DIR}/Engine/*.c"
    "${PROJECT_SOURCE_DIR}/Engine/*.h"
)

# file(GLOB_RECURSE EXAMPLES_SRC ${PROJECT_SOURCE_DIR}/Examples/*.c)
add_executable(${EXE_NAME} main.c ${EXAMPLE_SOURCES} ${ENGINE_SRC})

# SDL2::SDL2main needs to come first to compile with GCC 10.3.0
target_link_libraries(${EXE_NAME} PRIVATE glad::glad)
target_link_libraries(${EXE_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2)
target_link_libraries(${EXE_NAME} PRIVATE assimp::assimp)