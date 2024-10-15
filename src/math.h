#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "entities/camera.h"

class Maths {
public:
    // Function to create the transformation matrix
    static glm::mat4 createTransformationMatrix(const glm::vec3& translation, float rx, float ry, float rz, float scale) {
        glm::mat4 matrix = glm::mat4(1.0f); // Initialize as identity matrix
        matrix = glm::translate(matrix, translation); // Apply translation
        matrix = glm::rotate(matrix, glm::radians(rx), glm::vec3(1, 0, 0)); // Rotate around X axis
        matrix = glm::rotate(matrix, glm::radians(ry), glm::vec3(0, 1, 0)); // Rotate around Y axis
        matrix = glm::rotate(matrix, glm::radians(rz), glm::vec3(0, 0, 1)); // Rotate around Z axis
        matrix = glm::scale(matrix, glm::vec3(scale, scale, scale)); // Apply scaling
        return matrix;
    }

    // Function to create the view matrix based on the camera
    static glm::mat4 createViewMatrix(const Camera& camera) {
        glm::mat4 viewMatrix = glm::mat4(1.0f); // Initialize as identity matrix
        // Apply camera pitch and yaw rotations
        viewMatrix = glm::rotate(viewMatrix, glm::radians(camera.getPitch()), glm::vec3(1, 0, 0)); // Pitch
        viewMatrix = glm::rotate(viewMatrix, glm::radians(camera.getYaw()), glm::vec3(0, 1, 0)); // Yaw
        // Get camera position and apply inverse translation
        glm::vec3 cameraPos = camera.getPosition();
        glm::vec3 negativeCameraPos = -cameraPos;
        viewMatrix = glm::translate(viewMatrix, negativeCameraPos);
        return viewMatrix;
    }
};
