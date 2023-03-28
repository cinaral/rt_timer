# Template C++ Project
This is my personal C++ project template. I need reference solutions to test my programs occasionally, so this template uses reference data, which was generated using MATLAB.

# How to configure, compile and test with CMake
You can use the included scripts:
```bash
cd scripts/build
./clear.sh
./configure.sh
./build.sh
./test.sh
```

# How to compile on Windows using VS Code and MSYS2
1. Install MSYS2 and add to path.
2. Install mingw-w64 toolchain using MSYS2, ```pacman -S --needed base-devel mingw-w64-x86_64-toolchain```.
3. Use the build tasks, ```ctrl + shift + B```.
