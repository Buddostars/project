# Project- COMP3320

## Introduction
This is the COMP3320 Final Project for Group 1 due 22/10/24. As a team, we created Cows n Cars using OpenGL in C++ 
## About the Game
Objective:
- Hit Cows in Giraffes to score points within 30 seconds to achieve a high score.   
- Avoid rocks and the wall so you can get more points hitting giraffes with the cow
- Don't hit the cows to hard since you wont get them back
## Getting Started

This project uses VS Code to setup and runs the OpenGL, GLFW, GLAD, glm and assimp all in C++.

### Prerequisites
To run the game, you will need to have VS Code installed onto you machine.

-[VS Code](https://code.visualstudio.com)

### Running the Application

1. Clone the repository to your machine.

```bash
git clone https://github.com/Buddostars/project

```
2.  Compiling on mac

    1. In VSCode navigate to the main.cpp file.
    2. Run the file and select the option "C/C++: clang++ build and debug active file preLaunchTask: C/C++ clang++ macos ARM"
    3. Now the complile task should start and run the program afterwards.

    Make sure that only the macOS task is available in your tasks.json (.vscode/tasks.json)

    After compiling it you can run it again running the ./app in the projects root.

3. Compiling on window

    1. Install MinGW-w64 via [MSYS2](https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe) (Source/Further Info: [VS-Code CPP Setup](https://code.visualstudio.com/docs/cpp/config-mingw))
    2. In the MSYS2 terminal install the Toolchain:
    
    `pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`
    
    --> In the selection install all packages

    3. Open the urct64 application in C:\msys64 to open the terminal

    4. Run this code
    `C:/msys64/ucrt64/bin/g++.exe -std=c++17 -g -I(Path to project)/dependencies/include -L(Path to project)/dependencies/windows/library (Path to project)/src/*.cpp (Path to project)/src/glad.c (Path to project)/dependencies/windows/library/libglfw3dll.a (Path to project)/dependencies/windows/library/libassimp.dll.a -o (Path to project)/main.exe -Wno-deprecated`

    This will manually build the project

    Note: (Path to project) will be the path on where you have cloned the project

    5. Once built in the same terminal type `./main` to run it 

## Additional Information

For more detailed setup instructions, refer to the [vs_code_setup.md](vs_code_setup.md) file.

Enjoy the game!