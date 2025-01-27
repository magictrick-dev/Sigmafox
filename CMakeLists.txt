cmake_minimum_required(VERSION 3.21)

# --- Project Setup ------------------------------------------------------------
#
# Here, we designate the project name and the output directory for the build.
# Additionally, you can toggle the debug flag for production builds as well.
#

project(Sigmafox C CXX)
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
        "source/platform/win32/fileio.c"
        "source/platform/win32/system.c"
    )
endif (WIN32)

# --- Executable File Listings -------------------------------------------------
#
# Defines the executable and the files that need to be added to compile it.
#
 
add_executable(Sigmafox
    "source/main.c"

    "source/compiler/parser.h"
    "source/compiler/parser.c"
    "source/compiler/stringpool.h"
    "source/compiler/stringpool.c"
    "source/compiler/symboltable.h"
    "source/compiler/symboltable.c"
    "source/compiler/generation.h"
    "source/compiler/generation.c"
    "source/compiler/tokenizer.h"
    "source/compiler/tokenizer.c"
    "source/compiler/errorhandler.h"
    "source/compiler/errorhandler.c"
    
    "source/platform/fileio.h"
    "source/platform/system.h"

    "source/core/definitions.h"
    "source/core/arena.h"
    "source/core/arena.c"
    "source/core/pool.h"
    "source/core/pool.c"
    "source/core/cli.h"
    "source/core/cli.c"
    "source/core/memops.h"
    "source/core/memops.c"
    "source/core/resource.h"
    "source/core/resource.c"
    "source/core/generalalloc.h"
    "source/core/generalalloc.c"

    ${PLATFORM_FILES}
)

# --- Include Directories ------------------------------------------------------
#
# Defines which directories that are allowed to be bracket-included versus
# relative pathing.
#

target_include_directories(Sigmafox PUBLIC "source")
set_property(TARGET Sigmafox PROPERTY CXX_STANDARD 20)
set_property(TARGET Sigmafox PROPERTY C_STANDARD 17)
