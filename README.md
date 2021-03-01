# nifly
C++ NIF library for the Gamebryo/NetImmerse File Format.  
Created with a clean-room design.

### Features
- Reading and writing NIF files (Gamebryo/NetImmerse File Format)
- Cross platform
- Lots of helper functions
- Includes basics often used with 3D meshes
- Unknown NIF blocks are kept untouched where possible.
- No external dependencies (at this point)

### Usage/Building
- Simply include the headers/source in your project.
- You may use the included CMake list.
- Tested with MSVC++ 14.x (VS 2017) or higher
- Should work with most compilers. If not, please feel free to add an issue or make a PR.

### Libraries used
- [half - IEEE 754-based half-precision floating point library (v1.12.0)](http://half.sourceforge.net/)
- [Miniball (v3.0)](https://people.inf.ethz.ch/gaertner/subdir/software/miniball.html)

### Used by
- [BodySlide and Outfit Studio](https://github.com/ousnius/BodySlide-and-Outfit-Studio)
- [SSE NIF Optimizer](https://github.com/ousnius/SSE-NIF-Optimizer)
- [Cathedral Assets Optimizer](https://gitlab.com/G_ka/Cathedral_Assets_Optimizer)

### Credits
This library would not have been possible without the help of:
- [jonwd7](https://github.com/jonwd7)
- [Caliente](https://github.com/Caliente8)
- [NifTools team](https://www.niftools.org/)
