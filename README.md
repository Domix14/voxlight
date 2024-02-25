# Voxlight

Voxel raycasting engine

## Roadmap to v0.1.0
- ~~voxel raycasting~~
- raytraced lighting
- physics/collision

## Building

Current build configuration supports only Windows platform.

### Requirements
 - MSCV or GCC with C++20 support
 - Python 3.x
 - CMake >=3.26

Build system uses CMake presets and toolchains to configure project.

```bat
mkdir build
cd build

cmake .. --preset=windows-msvc
cd windows-msvc

cmake --build .
```

To build with GCC, change 'windows-msvc' to 'windows-gcc'.


