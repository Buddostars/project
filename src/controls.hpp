#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

void computeMatricesFromInputs(GLFWwindow* window, float deltaTime);

//third person view
void setThirdPersonView(const glm::vec3& position, const glm::vec3& direction);

void updateModelPosition(GLFWwindow* window, float deltaTime);

#endif