CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

PROJECT(cosyproject)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_BUILD_TYPE Debug)

ADD_EXECUTABLE(cosyproject
    "tests\sample.cpp"
    "tests\includes\math.hpp"
    "tests\includes\intrinsics.hpp"
)
