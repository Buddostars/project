// display.cpp
#include "display.h"
#include <glad/glad.h>
#include <iostream>

// Initialize the static member for the window title
const char* DisplayManager::TITLE = "Our First Display";

// Function to create the window and set up OpenGL
#include <GLFW/glfw3.h> // Include GLFW header for full screen functionality

GLFWwindow* DisplayManager::createDisplay() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Set GLFW to use OpenGL version 3.2, Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS

    // Retrieve the primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Create a full-screen window
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, TITLE, monitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Define the viewport dimensions
    glViewport(0, 0, mode->width, mode->height); // Use full screen dimensions

    return window;
}


// Function to update the display (swap buffers and poll events)
void DisplayManager::updateDisplay(GLFWwindow* window) {
    glfwSwapBuffers(window); // Swap front and back buffers
    glfwPollEvents();        // Poll for and process events

    // Cap frame rate
    glfwSwapInterval(1); // Enables V-Sync to cap FPS
}

// Function to close the display and clean up
void DisplayManager::closeDisplay(GLFWwindow* window) {
    glfwDestroyWindow(window); // Destroy the window
    glfwTerminate();           // Terminate GLFW
}
