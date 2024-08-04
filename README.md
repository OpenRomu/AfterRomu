# AfterRomu

AfterRomu is an unofficial source port and modernization of the RomuStrike
v140 version.

## Building

AfterRomu is currently dependent on the following tools and libraries:

- FreeGLUT
- CMake (for Windows)
- Visual Studio 2022
- DirectX 8
- Windows SDK

### Building Dependencies

AfterRomu needs some configuration before being able to be built.

#### Git Submodules

Before building the dependencies, make sure to update the Git submodules:

```powershell
> git submodule update --init --recursive
```

This will ensure that all the necessary submodules are properly checked out
and ready for building.

#### FreeGLUT

To build FreeGLUT, you'll need to have CMake installed on your Windows system.
This can be done using Winget or Chocolatery.

Follow these steps:
1. Open a PowerShell or Command Prompt window and navigate to the `freeglut`
   directory.
2. Create a new `build` directory and change into it:
   ```powershell
   > cd freeglut
   > mkdir build && cd build
   ```
3. Generate the project files for Visual Studio 2022 (32-bit):
   ```powershell
   > cmake .. -G "Visual Studio 17 2022" -A Win32
   ```
4. Open the generated `freeglut.sln` solution file in Visual Studio 2022.
5. In Visual Studio, select "Build" and then "Build ALL_BUILD" to compile the
FreeGLUT library.

After completing these steps, you should have the necessary FreeGLUT library
built and ready to use in your AfterRomu project.

### Building AfterRomu

To build AfterRomu, follow these steps:

1. Open the `AfterRS.sln` solution file in Visual Studio 2022.
2. In the Visual Studio menu, go to "Build" and select "Build AfterRS" to
   build both the debug and release versions of the AfterRomu application.

After the build process is complete, you should have the AfterRomu executable
ready to run.

### Code Formatting

To maintain a consistent code style throughout the game project, we utilize
**Clang-Format** for formatting our codebase. This tool helps ensure that our
code adheres to a specified style guide, making it easier to read and maintain.

#### Using Docker for Clang-Format

To simplify the setup process and avoid the need to install a compatible
version of Clang-Format on your local machine, we provide a Docker container.
Follow the steps below to build and run the Docker container for code
formatting.

#### Build the Docker Image

First, you need to build the Docker image. Open your terminal and run the
following command:
```bash
$ docker build -t afterromu:latest .
```

#### Run the Docker Container

Next, you can run the Docker container. Depending on your operating system,
use one of the following commands:
```bash
# Windows
$ docker run -v ${PWD}:/opt --rm -it --entrypoint bash afterromu
# Bash / Linux
$ docker run -v $PWD:/opt --rm -it --entrypoint bash afterromu
```

#### Format the Code

Once inside the Docker container, you can format the code by executing the
following command:
```bash
$ find src/ -regex '.*\.\(cpp\|hpp\|c\|h\)' -exec clang-format -style=file -i {} \;
```

This command will search for all C++ source files (.cpp, .hpp, .c, .h)
in the src/ directory and apply the formatting rules specified in the
.clang-format configuration file.

## License

The source code for RomuStrike includes code from the following sources:
- Half-Life, from Valve Software/ID Software (all rights reserved)
- The "HL Rendering Engine", a minimal engine inspired by Half-Life, created
  by Bart Sekura

Additionally, the repository includes utility libraries provided by Nate Miller,
which are licensed under the GPLv2+ license.
