cmake_minimum_required(VERSION 3.0.0)
project(FCC_OpenGL_Tutorial)

#set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

set(EXE_NAME "MAIN")

include_directories(
        ${PROJECT_SOURCE_DIR}/Engine
)

file(GLOB ENGINE_SRC
    "${PROJECT_SOURCE_DIR}/Engine/*.c"
    "${PROJECT_SOURCE_DIR}/Engine/*.h"
)

add_executable(${EXE_NAME} main.c ${ENGINE_SRC})

# SDL2::SDL2main needs to come first to compile with GCC 10.3.0
target_link_libraries(${EXE_NAME} PRIVATE glad::glad)
target_link_libraries(${EXE_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2)