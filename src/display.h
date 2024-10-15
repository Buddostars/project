// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders/shader.h"
#include "controls.hpp"
#include "models/object.h"
class DisplayManager {
public:
    static GLFWwindow* createDisplay();
    static void updateDisplay(GLFWwindow* window);
    static void closeDisplay(GLFWwindow* window);

private:
    static const int WIDTH = 1920;
    static const int HEIGHT = 1080;
    static const int FPS_CAP = 60;
    static const char* TITLE;
};

#endif // DISPLAY_H
