#include "controls.hpp"

// Constructor implementation
Camera::Camera() 
    : position(vec3(0, 0, 5)), horizontalAngle(3.14f), verticalAngle(0.0f),
      speed(5.0f), mouseSpeed(0.005f), initialFoV(45.0f) {
    ProjectionMatrix = glm::perspective(radians(initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
}

// Method to compute matrices based on inputs
void Camera::computeMatricesFromInputs(GLFWwindow* window) {
    static double lastTime = glfwGetTime();

    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, 1024/2, 768/2); // Reset mouse position for next frame

    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024/2 - xpos);
    verticalAngle   += mouseSpeed * float(768/2 - ypos);
    verticalAngle = std::clamp(verticalAngle, -1.5f, 1.5f); // Clamp vertical angle to prevent flipping

    // Direction calculation
    vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle), 
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right and up vectors
    vec3 right = vec3(
        sin(horizontalAngle - 3.14f/2.0f), 
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    vec3 up = glm::cross(right, direction);

    // Move camera based on key inputs
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        position += direction * deltaTime * speed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        position -= direction * deltaTime * speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        position += right * deltaTime * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        position -= right * deltaTime * speed;
	// Add vertical movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Move up
        position += up * deltaTime * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Move down
        position -= up * deltaTime * speed;

    // Update matrices
    ViewMatrix = glm::lookAt(position, position + direction, up);
    lastTime = currentTime; // Update last time
}

// Getter methods for matrices
mat4 Camera::getViewMatrix() const {
    return ViewMatrix;
}

mat4 Camera::getProjectionMatrix() const {
    return ProjectionMatrix;
}
