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

The project file structure diagram describes the basic layout of how files are laid out.
Take careful note of platform directory since it is the only directory in Sigmafox that
has special meaning to the build system. Header files within this directory are the only
files meant to be included by the front-end APIs; sub-directories like win32, unix, or macosx
are not meant to be directly imported.

<p align="center"><img src="./documentation/images/file_layout_structure.svg" /></p>

-   **source/** Contains runtime specific files and library directories. Files of note in
    this directory are `main.cpp` which serves as the application entry point, and `runtime.h`
    which describes the runtime environment that main directly invokes at startup.

-   **source/core** Contains commonly used libraries for the project.

    -   **source/core/utilities** A set of libraries such as `array`, `string`, and various
        other utilities such as custom allocators. You can selective choose which libraries
        to use from this directory or include `<core/utilities.h>` for them all.

-   **source/compiler** Contains implementation files for the Sigmafox compiler.

-   **source/platform** Contains a set of header definition files that each platform must
    implement in their respective folders. CMake will automatically detect which OS the
    project is built on and appropriately include the implementation files.

# Documentation

-   **[Utilites: Allocators](./documentation/allocators.md)**

    Various memory allocators and memory management utilities.

-   **[Compiler: Scanner](./documentation/scanner.md)**

    Routines for converting raw text files into tokenized outputs for further parsing.

