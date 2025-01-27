# Sigmafox Programming Language

Sigmafox is a transpiled scripting language designed to aid in the development of
complex physics applications by abstracting out low-level detail into a simplified
scripting language. Sigmafox transpiles to C++ for performance and extensibility.
Sigmafox supports n-th order automatic differentiation and parallel computation in
cluster computation environments.

### Project Status

The current state of the project is a work in-progress. As such, it is not in a state
for use in development. Currently, Sigmafox is currently being refactored in C++ for better clarity
and scaleability. This can be tracked in the `SFRefactor/` subfolder. If you are interested
to see the current state of the project, visit the `version-alpha` branch where the commits
are being pushed to.

Sigamfox is being developed on the Windows platform, with the intention to port to
UNIX and MacOSX once the core language implementation is complete. Therefore, you will
not be able to compile this on any platform but Windows with MSVC installed.

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

2.  Build the project.

    ```
    cmake --build ./comp
    ```

    This will build the compiler from source. The build output will be in the calling directory under
    `./bin`. You can run executable from there or move the contents of that directory
    somewhere more permanent. You may freely recompile the project using this command
    if you make any changes to the source code.

### Using Sigmafox

In order to build sigmafox scripts, provide the file path as such:

```
sigmafox my_script.fox
```

This will automatically convert the script to C++ and generate the `CMakeLists.txt` 
file needed to compile it.

# License

Sigmafox is developed and owned by Northern Illinois University. Please contact
the Department of Physics at NIU for more information. You can also reach out to
the current maintainer at `Z1836870@students.niu.edu`.
