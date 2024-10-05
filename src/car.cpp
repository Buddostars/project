#include "car.hpp"
#include <iostream>

Car::Car() : position(glm::vec3(25.0f, 0.0f, 0.0f)), speed(10.0f) {}

void Car::update(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position.z -= speed * 0.01f;  // Move forward along the Z-axis
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.z += speed * 0.01f;  // Move backward along the Z-axis
    }
}

void Car::render() {
    // Render the car model here
}

glm::vec3 Car::getPosition() const {
    return position;
}
