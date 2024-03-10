cmake_minimum_required(VERSION 3.21)

project(POSY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
add_compile_definitions(POSY_DEBUG_BUILD=1)

add_executable(POSY
    "source/main.cpp"
    "source/cli.h"
    "source/cli.cpp"

    "source/core.h"
    "source/core/debugging.h"
    "source/core/primitives.h"
    "source/core/allocator.cpp"
)

target_include_directories(POSY PUBLIC
    "source"
    "vendor"
)

