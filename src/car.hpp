#ifndef CAR_HPP
#define CAR_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "model.hpp"
#include "shader.h"

class Car {
public:
    // Constructor
    Car(Model& model);

    // Functions
    void update(float deltaTime, GLFWwindow* window);
    void draw(Shader& shader);


    // Getters 
    glm::vec3 getPosition() const;
    glm::vec3 getForwardDirection() const;

private:
    Model model; // Car model
    glm::vec3 position;
    float speed;
    float steeringAngle;
    float turningSpeed;
};

#endif // CAR_HPP