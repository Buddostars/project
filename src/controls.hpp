#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // For std::clamp

using namespace glm;

class Camera {
public:
    // Camera attributes
    vec3 position;
    float horizontalAngle;
    float verticalAngle;
    float speed;
    float mouseSpeed;
    float initialFoV;
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;

    // Constructor
    Camera();

    // Method to compute matrices based on inputs
    void computeMatricesFromInputs(GLFWwindow* window);

    // Getter methods for matrices
    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix() const;
};

#endif // CONTROLS_HPP
