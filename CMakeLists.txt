cmake_minimum_required(VERSION 3.21)

project(Sigmafox)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
add_compile_definitions(SIGMAFOX_DEBUG_BUILD=1)

add_executable(Sigmafox
    "source/main.cpp"
    
    "source/lexer.h"
    "source/source.h"
    "source/commandline.h"
    "source/lexer.cpp"
    "source/source.cpp"
    "source/commandline.cpp"

    "source/core.h"
    "source/core/debugging.h"
    "source/core/primitives.h"
    "source/core/fileio.h"
    "source/core/allocator.cpp"
    "source/core/fileio.cpp"
)

target_include_directories(Sigmafox PUBLIC
    "source"
    "vendor"
)

