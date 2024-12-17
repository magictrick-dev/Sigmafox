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
        "source/platform/win32/filesystem.cpp"
        "source/platform/win32/system.cpp"
    )
endif (WIN32)

# --- Executable File Listings -------------------------------------------------
#
# Defines the executable and the files that need to be added to compile it.
#
 
add_executable(Sigmafox
    "source/main.cpp"
    "source/definitions.hpp"
    "source/environment.hpp"

    "source/platform/filesystem.hpp"
    "source/platform/system.hpp"

    "source/utilities/path.hpp"
    "source/utilities/path.cpp"
    "source/utilities/cli.hpp"
    "source/utilities/cli.cpp"

    "source/compiler/tokenizer.hpp"
    "source/compiler/tokenizer.cpp"
    "source/compiler/parser.hpp"
    "source/compiler/parser.cpp"
    "source/compiler/syntaxtree.hpp"
    "source/compiler/syntaxtree.cpp"
    "source/compiler/dependencygraph.hpp"
    "source/compiler/dependencygraph.cpp"
    "source/compiler/errorhandler.hpp"
    "source/compiler/symbols.hpp"
    "source/compiler/symbolstack.hpp"

    "source/compiler/syntaxnode.hpp"
    "source/compiler/nodes/include.hpp"
    "source/compiler/nodes/main.hpp"
    "source/compiler/nodes/module.hpp"
    "source/compiler/nodes/root.hpp"

    ${PLATFORM_FILES}
)

# --- Include Directories ------------------------------------------------------
#
# Defines which directories that are allowed to be bracket-included versus
# relative pathing.
#

target_include_directories(Sigmafox PUBLIC "source")
set_property(TARGET Sigmafox PROPERTY CXX_STANDARD 20)

if (WIN32)
    TARGET_LINK_LIBRARIES(Sigmafox Shlwapi.lib)
endif(WIN32)
