# Sigmafox Programming Language

Sigmafox is a transpiled scripting language designed to aid in the development of
complex physics applications by abstracting out low-level detail into a simplified
scripting language. Sigmafox transpiles to C++ for performance and extensibility.
Sigmafox is not an interpretted scripting language, so it requires both a transpilation
and a compilation steps to execute code written in Sigmafox.

# Documentation

### Building Sigmafox

In order to build Sigmafox from source, you will need [the latest version of CMake](https://cmake.org/)
installed on your system and accessible on the command line. Clone the repository to a
directory of your choice and navigate to that directory through the command line. Sigmafox
currently requires no additional SDKs or dependencies to work.

1. Configure the project.

    ```
    cmake -B ./comp
    ```

    This will create the required build files needed for your current platform. For
    Windows, you will need [Visual Studio's C/C++ MSVC Compiler](https://visualstudio.microsoft.com/#vs-section)
    in order to do this. On most UNIX platforms, [the GCC compiler is installed by default](https://gcc.gnu.org/).
    Currently, MacOSX is not supported. You only need to generate the project files once.
    In some rare cases during development, the project files might mismatch against the
    source files. In that event, you can delete the project folder `./comp` and rerun the
    above command to regenerate the build files.

    The CMakeLists.txt automatically checks for the platform you are on and switches which
    files it needs to compile for your system. See **Platform Abstraction API** for more
    information on how that works.

2.  Build the project.

    ```
    cmake --build ./comp
    ```

    This will compile the project. The build output will be in the calling directory under
    `./bin`. You can run executable from there or move the contents of that directory
    somewhere more permanent. You may freely recompile the project using this command
    if you make any changes to the source code.

### Project File Structure

Implementation files are location `source/` and are organized as described below:

-   `source/` The root directory containing runtime code. Generally speaking, files
    that reside within this directory are non-library files needed to execute the
    program. Files of note include `main.cpp` which contains the program entry point
    and `runtime.h` and `runtime.cpp` which contain the runtime routines that main
    invokes at launch.

-   `source/compiler` Contains all implementation and header files regarding the compilation
    routines needed to properly transform Sigmafox source files to C++ source files.

-   `source/core` Contains commonly included helper libraries and definitions.

    - `source/core/utilities` Library implementations such as string, vector, and allocators
      reside in this directory. You can manually include which utility library you need or
      you can include `source/utilities.h` to get them all.

-   `source/platform` The root directory for platform-specific header files are here.
    Each platform has its own directory and implementations for these source files.
    While the majority of the code base for Sigmafox is platform agnostic, some operations
    require OS calls to manage files, memory, and other critical OS-level operations. Rather 
    than writing a dense wrapper for each operating system, the code that drives these 
    functions are housed in separate folders that get dynamically added to the build using CMake.

    <p align="center"><img src="./documentation/images/platform_file_layout.svg" /></p>

    The platform header files describe what the implementation files need to implement. The
    front-end API isn't aware how these platform abstractions are working under the hood, only
    that they perform the required operations as outlined by the header file itself. When adding
    a platform, make sure to properly implement each API carefully so that the desired results
    are achieved. The idea behind this design is to maintain low-level access to the OS calls
    so that more, well-developed APIs can abstract them further such that they require little
    modification beyond the initial implementation.

# Documentation Index

-   **[Memory Allocators](./documentation/allocators.md)**

    Describes the various custom memory allocators and strategies that Sigmafox utilizes.

-   **Platform APIs**

    Some operations require access to operating system calls to leverage numerous features
    that improve reliability and speed. These platform APIs are implemented with respect
    to the OS that they're built on (see [File Structure](#project-file-structure) for more
    information about this).

    -   **[Platform File Input/Output API](./documentation/fileio.md)**

        Defines various file input/output routines to read and write files.

    -   **[Platform Virtual Memory API](./documentation/virtmem.md)**

        Defines functions that help access low-level memory pages for more efficient
        memory useage.

-   **[Scanner API](./documentation/scanner.md)**

    The scanner API is what the transpiler uses in the first stage of the compiler to tokenize
    raw source files.

