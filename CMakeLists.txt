cmake_minimum_required(VERSION 3.16)
project(OpenGLProject)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)

# Compiler options for clang++
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -g -fdiagnostics-color=always -Wno-deprecated)
endif()

# Define include directories
include_directories(${CMAKE_SOURCE_DIR}/dependencies/include)

# Source files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/controls.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
)

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Platform-specific settings
if(APPLE)
    # macOS settings
    set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/dependencies/osx/library)

    # Add macOS frameworks
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    find_library(COREFUNDATION_FRAMEWORK CoreFoundation)
    find_library(OPENGL_FRAMEWORK OpenGL)

    # Link macOS frameworks and libraries
    target_link_libraries(${PROJECT_NAME}
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
        ${COREFUNDATION_FRAMEWORK}
        ${OPENGL_FRAMEWORK}
        ${LIBRARY_PATH}/libglfw.3.dylib
        ${LIBRARY_PATH}/libassimp.5.4.3.dylib
    )

    # Set runtime path for macOS
    set(CMAKE_BUILD_RPATH ${LIBRARY_PATH})

elseif(WIN32)
    # Windows settings
    set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/dependencies/windows/library)

    # Link Windows libraries
    target_link_libraries(${PROJECT_NAME}
        ${LIBRARY_PATH}/libglfw3dll.a
        ${LIBRARY_PATH}/libassimp.dll.a
    )

    # Set runtime path for Windows
    set(CMAKE_BUILD_RPATH ${LIBRARY_PATH})

endif()
