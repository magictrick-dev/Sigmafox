cmake_minimum_required(VERSION 3.21)

project(Sigmafox)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
add_compile_definitions(SIGMAFOX_DEBUG_BUILD=1)

set(COMMON_LIBRARIES
    "common/core.h"
    "common/core/debugging.h"
    "common/core/primitives.h"
    "common/core/fileio.h"
    "common/core/allocator.cpp"
    "common/core/fileio.cpp"
)

add_executable(Sigmafox
    "source/main.cpp"
    
    "source/lexer.h"
    "source/source.h"
    "source/commandline.h"
    "source/lexer.cpp"
    "source/source.cpp"
    "source/commandline.cpp"

    ${COMMON_LIBRARIES}
)

target_include_directories(Sigmafox PUBLIC
    "./source"
    "./common"
    "./vendor"
)

