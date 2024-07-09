cmake_minimum_required(VERSION 3.21)

# --- Project Setup ------------------------------------------------------------
#
# Here, we designate the project name and the output directory for the build.
# Additionally, you can toggle the debug flag for production builds as well.
#

project(Sigmafox)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
add_compile_definitions(SIGMAFOX_DEBUG_BUILD=1)

# --- Platform Detection -------------------------------------------------------
#
# Determines which platform specific files to compile for the current target
# platform. Generally, you shouldn't list any non-platform specific source files
# here and instead you should list them in the "executable file listings".
#

set(PLATFORM_FILES)
if (WIN32)
    set(PLATFORM_FILES
        "source/platform/win32/fileio.cpp"
        "source/platform/win32/system.cpp"
    )
endif (WIN32)

# --- Executable File Listings -------------------------------------------------
#
# Defines the executable and the files that need to be added to compile it.
#
 
add_executable(Sigmafox
    "source/main.cpp"
    "source/runtime.h"
    "source/runtime.cpp"

    "source/compiler/token.h"
    "source/compiler/token.cpp"
    "source/compiler/scanner.h"
    "source/compiler/scanner.cpp"
    "source/compiler/parser.h"
    "source/compiler/parser.cpp"
    "source/compiler/environment.h"
    "source/compiler/environment.cpp"
    
    "source/platform/fileio.h"
    "source/platform/system.h"

    "source/core/utilities.h"
    "source/core/utilities/array.h"
    "source/core/utilities/hashtable.h"
    "source/core/utilities/hashtable.cpp"
    "source/core/utilities/allocator.h"
    "source/core/utilities/allocator.cpp"

    ${PLATFORM_FILES}
)

# --- Include Directories ------------------------------------------------------
#
# Defines which directories that are allowed to be bracket-included versus
# relative pathing.
#

target_include_directories(Sigmafox PUBLIC
    "source"
)

set_property(TARGET Sigmafox PROPERTY CXX_STANDARD 20)
