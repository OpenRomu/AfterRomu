# AfterRomu

AfterRomu is an unofficial source port and modernization of the RomuStrike
v140 version.

## Building

AfterRomu is currently dependent on the following tools and libraries:

- FreeGLUT
- CMake (for Windows)
- Visual Studio 2026
- DirectX 8
- Windows SDK
- MSXMLv4 SP2 SDK

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
   > cmake .. -G "Visual Studio 18 2026" -A Win32
   ```
4. Open the generated `freeglut.sln` solution file in Visual Studio 2022.
5. In Visual Studio, select "Build" and then "Build ALL_BUILD" to compile the
FreeGLUT library.

After completing these steps, you should have the necessary FreeGLUT library
built and ready to use in your AfterRomu project.

### Building AfterRomu

To build AfterRomu, follow these steps:

1. Open the `AfterRS.sln` solution file in Visual Studio 2026.
2. In the Visual Studio menu, go to "Build" and select "Build AfterRS" to
   build both the debug and release versions of the AfterRomu application.

After the build process is complete, you should have the AfterRomu executable
ready to run.

## License

The source code for RomuStrike includes code from the following sources:
- The "HL Rendering Engine", a minimal engine inspired by Half-Life, created
  by Bart Sekura

Additionally, the repository includes utility libraries provided by Nate Miller,
which are licensed under the GPLv2+ license.
