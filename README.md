# nifly
C++ NIF library for the NetImmerse File Format (NetImmerse, Gamebryo, Creation Engine).  
Created with a clean-room design.

[![CMake](https://github.com/ousnius/nifly/actions/workflows/cmake.yml/badge.svg)](https://github.com/ousnius/nifly/actions/workflows/cmake.yml)

### Features
- Reading and writing NIF files (NetImmerse, Gamebryo, Creation Engine)
- Cross platform (Windows, Linux, macOS)
- No external dependencies (tests optional)
- Includes basic functionality often used for 3D meshes
- NIF blocks unknown to the library are kept untouched
- Lots of other helper functions
- Current file support:
  - Fallout 3
  - Fallout: New Vegas
  - Fallout 4
  - Fallout 4 VR
  - The Elder Scrolls V: Skyrim
  - The Elder Scrolls V: Skyrim Special Edition
  - The Elder Scrolls V: Skyrim VR

### Building
- C++ 17 compiler required
- Use the included CMake lists for building - or simply include the headers and source in your project
- (optional) To get Catch2 for running the tests, install its [CMake package](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository) (or get it via vcpkg):
```bash
git clone -b v2.x https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install  #with admin elevation (or sudo) depending on your cmake prefix path
```

### Libraries used
- [half - IEEE 754-based half-precision floating point library (v1.12.0)](http://half.sourceforge.net/) for 16-bit floats
- [Miniball (v3.0)](https://people.inf.ethz.ch/gaertner/subdir/software/miniball.html) for generating bounding spheres
- [Catch2 (v2.x branch)](https://github.com/catchorg/Catch2/tree/v2.x) for running tests

### nifly is used by
- [BodySlide and Outfit Studio](https://github.com/ousnius/BodySlide-and-Outfit-Studio)
- [SSE NIF Optimizer](https://github.com/ousnius/SSE-NIF-Optimizer)
- [Cathedral Assets Optimizer](https://gitlab.com/G_ka/Cathedral_Assets_Optimizer)

### Credits
- [Contributors to nifly](https://github.com/ousnius/nifly/graphs/contributors)
- [ousnius](https://github.com/ousnius)
- [jonwd7](https://github.com/jonwd7)
- [Caliente](https://github.com/Caliente8)
- [NifTools team](https://www.niftools.org/)
