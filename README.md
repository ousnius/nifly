# nifly
C++ NIF library for the Gamebryo/NetImmerse File Format.  
Created with a clean-room design.

[![CMake](https://github.com/ousnius/nifly/actions/workflows/cmake.yml/badge.svg)](https://github.com/ousnius/nifly/actions/workflows/cmake.yml)

### Features
- Reading and writing NIF files (Gamebryo/NetImmerse File Format)
- Cross platform
- Lots of helper functions
- Includes basics often used with 3D meshes
- Unknown NIF blocks are kept untouched
- No external dependencies except for the test framework

### Usage/Building
- C++ 17 compiler required
- CMake lists included, or simply include the headers/source in your project
- To get Catch2, install the [CMake package](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository) (or vcpkg):
```bash
git clone -b v2.x https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install  #with admin elevation (or sudo) depending on your cmake prefix path
```

### Libraries used
- [half - IEEE 754-based half-precision floating point library (v1.12.0)](http://half.sourceforge.net/)
- [Miniball (v3.0)](https://people.inf.ethz.ch/gaertner/subdir/software/miniball.html)
- [Catch2 (v2.x)](https://github.com/catchorg/Catch2/) as the test framework

### Used by
- [BodySlide and Outfit Studio](https://github.com/ousnius/BodySlide-and-Outfit-Studio)
- [SSE NIF Optimizer](https://github.com/ousnius/SSE-NIF-Optimizer)
- [Cathedral Assets Optimizer](https://gitlab.com/G_ka/Cathedral_Assets_Optimizer)

### Credits
This library would not have been possible without the help of:
- [jonwd7](https://github.com/jonwd7)
- [Caliente](https://github.com/Caliente8)
- [NifTools team](https://www.niftools.org/)
