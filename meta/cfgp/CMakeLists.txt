cmake_minimum_required(VERSION 3.21)

project(cfgp)

set(COMMON_LIBRARIES
    "../../common/core.h"
    "../../common/core/debugging.h"
    "../../common/core/primitives.h"
    "../../common/core/fileio.h"
    "../../common/core/allocator.cpp"
    "../../common/core/fileio.cpp"
)

add_executable(cfgp
    "main.cpp"
    ${COMMON_LIBRARIES}
)

target_include_directories(cfgp PUBLIC
    "../../common"
)

