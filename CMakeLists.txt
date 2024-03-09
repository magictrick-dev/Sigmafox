cmake_minimum_required(VERSION 3.21)

project(POSY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
set(POSY_DEBUG_BUILD 1)

add_executable(POSY
    "source/main.cpp"
)

target_include_directories(POSY PUBLIC
    "source"
    "vendor"
)

