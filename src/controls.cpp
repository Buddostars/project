#include "controls.hpp"

// Constructor implementation
Camera::Camera() 
    : position(vec3(0, 5, 10)), initialFoV(45.0f) {
    ProjectionMatrix = glm::perspective(radians(initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
}

// Method to update the camera matrices to follow the car
void Camera::computeMatricesFromInputs(GLFWwindow* window, const vec3& carPosition, const vec3& carForward) {
    
    // Define an offset for the camera behind and above the car
    vec3 cameraOffset = vec3(0.0f, 2.0f, -7.0f);

    // Calculate the new camera position based on the car position and direction
    position = carPosition - carForward * cameraOffset.z + vec3(0.0f, cameraOffset.y, 0.0f);

    //Make the camera look at the car
    ViewMatrix = glm::lookAt(position, carPosition, vec3(0.0f, 1.0f, 0.0f));
}

// Getter methods for matrices
mat4 Camera::getViewMatrix() const {
    return ViewMatrix;
}

mat4 Camera::getProjectionMatrix() const {
    return ProjectionMatrix;
}
