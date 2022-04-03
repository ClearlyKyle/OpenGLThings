cmake_minimum_required(VERSION 3.10)
project(FCC_OpenGL_Tutorial)

#if(MSVC)
#    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
#endif()

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

set(EXE_NAME "MAIN")

include_directories(
        ${PROJECT_SOURCE_DIR}/Engine
        ${PROJECT_SOURCE_DIR}/Engine/libs
        ${PROJECT_SOURCE_DIR}/Examples
)

file(GLOB ENGINE_SRC
    "${PROJECT_SOURCE_DIR}/Engine/*.c"
    "${PROJECT_SOURCE_DIR}/Engine/*.h"
)

file(GLOB EXAMPLES_SRC
    "${PROJECT_SOURCE_DIR}/Examples/*.c"
    "${PROJECT_SOURCE_DIR}/Examples/*.h"
)

add_executable(${EXE_NAME} main.c ${EXAMPLES_SRC} ${ENGINE_SRC})

# SDL2::SDL2main needs to come first to compile with GCC 10.3.0
target_link_libraries(${EXE_NAME} PRIVATE glad::glad)
target_link_libraries(${EXE_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2)