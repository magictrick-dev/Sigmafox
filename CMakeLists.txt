cmake_minimum_required(VERSION 3.21)

# --- Project Setup ------------------------------------------------------------
#
# Here, we designate the project name and the output directory for the build.
# Additionally, you can toggle the debug flag for production builds as well.
#

project(Sigmafox)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(SIGMAFOX_DEBUG_BUILD=1)

# Enable debug symbols.
set(CMAKE_BUILD_TYPE Debug)

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
if (UNIX)
    set(PLATFORM_FILES
        "source/platform/unix/filesystem.cpp"
        "source/platform/unix/system.cpp"
    )
endif (UNIX)

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
    "source/utilities/string.hpp"
    "source/utilities/vector.hpp"
    "source/utilities/memoryalloc.hpp"
    "source/utilities/memoryalloc.cpp"

    "source/compiler/compiler.hpp"
    "source/compiler/compiler.cpp"
    "source/compiler/environment.hpp"
    "source/compiler/environment.cpp"
    "source/compiler/graph.hpp"
    "source/compiler/graph.cpp"
    "source/compiler/reference.hpp"
    "source/compiler/reference.cpp"
    
    "source/compiler/parser/node.hpp"
    "source/compiler/parser/node.cpp"
    "source/compiler/parser/statements.hpp"
    "source/compiler/parser/statements.cpp"
    "source/compiler/parser/expressions.hpp"
    "source/compiler/parser/expressions.cpp"
    "source/compiler/parser/parser.hpp"
    "source/compiler/parser/parser.cpp"
    "source/compiler/parser/visitor.hpp"
    "source/compiler/parser/visitor.cpp"

    "source/compiler/parser/validators/exprtype.hpp"
    "source/compiler/parser/validators/exprtype.cpp"

    "source/compiler/tokenizer/token.hpp"
    "source/compiler/tokenizer/token.cpp"
    "source/compiler/tokenizer/tokenizer.hpp"
    "source/compiler/tokenizer/tokenizer.cpp"

    "source/compiler/symbols/symbol.hpp"
    "source/compiler/symbols/symbol.cpp"
    "source/compiler/symbols/table.hpp"
    "source/compiler/symbols/table.cpp"
    "source/compiler/symbols/table.hpp"
    "source/compiler/symbols/table.cpp"

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
    TARGET_LINK_LIBRARIES(Sigmafox Shlwapi.lib Shell32.lib)
endif(WIN32)
