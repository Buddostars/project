## Run our OpenGL Code in Windows in VS Code

Pretending you have a stock windows (x86) computer.

1. Install MinGW-w64 via [MSYS2](https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe) (Source/Further Info: [VS-Code CPP Setup](https://code.visualstudio.com/docs/cpp/config-mingw))
2. In the MSYS2 terminal install the Toolchain:
`pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`
--> In the selection install all packages

### Run/Debug with VS Code

3. Install the C/C++ Extesion in VS-Code
4. Run (F5) should now Build and Run the program.

### Alternative: using CMake

3. in the repo run `cmake -G "MinGW Makefiles" .`
4. afterwards run `cmake --build .`
5. Run the program `./OpenGLProject.exe`

### Compile third party libs

3. Clone repo of the library
4. in the repo run `cmake -G "MinGW Makefiles" .`
5. afterwards run `cmake --build .`

## Install CMake

1. Install [Cmake](https://cmake.org/download/)
2. Run `cmake -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_C_COMPILER=C:/msys64/ucrt64/bin/gcc.exe path/to/cmakelists.txt` the path to the compilers should normaly fit, also u have to change the path to the cmakelist.txt of the repo you want to compile. This needs to be run the first time to setup the path to the compilers, afterwards you just run the the commands above.