#include "car.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


Car::Car(Model& carModel) 
    :model(carModel), position(0.0f, 0.0f, 0.0f), speed(5.0f), steeringAngle(0.0f), turningSpeed(90.0f) {}

void Car::update(float deltaTime, GLFWwindow* window) {
    bool isTurning = false; // Check if the car is turning
    bool forward;
    bool reverse;

    // Extract the car's forward direction from the transformation matrix
    glm::vec3 forwardDirection = getForwardDirection();

    
    // Update car position based on direction and speed
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        forward = true;
        position -= forwardDirection * speed * deltaTime; 
    }
    else forward = false;
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        reverse = true;
        position += forwardDirection * speed * deltaTime; 
    }
    else reverse = false;

    // Handle car turning
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (forward || reverse)) {
        if (forward) {
            steeringAngle += turningSpeed * deltaTime; // Move left
        }
        else if (reverse) {
            steeringAngle -= turningSpeed * deltaTime; // Move right
        }
        isTurning = true;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (forward || reverse)) {
        if (forward) {
            steeringAngle -= turningSpeed * deltaTime; // Move right
        }
        else if (reverse) {
            steeringAngle += turningSpeed * deltaTime; // Move left
        }
        isTurning = true;
    }
}

void Car::draw(Shader& shader) {
    glm::mat4 carModelMatrix = glm::mat4(1.0f);
    carModelMatrix = glm::translate(carModelMatrix, position); // Position of car

    // Rotate the car 180 degrees along y axis to flip then apply the steering angle
    carModelMatrix = glm::rotate(carModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    carModelMatrix = glm::rotate(carModelMatrix, glm::radians(steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    carModelMatrix = glm::scale(carModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale car if necessary

    shader.setMat4("model", carModelMatrix);
    model.draw(shader);
}

glm::vec3 Car::getPosition() const {
    return position;
}

glm::vec3 Car::getForwardDirection() const {
    return glm::vec3(sin(glm::radians(steeringAngle)), 0.0f, cos(glm::radians(steeringAngle)));
}
