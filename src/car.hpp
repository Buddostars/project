#ifndef CAR_HPP
#define CAR_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "model.hpp"
#include "shader.h"
#include "ExhaustSystem.h"
#include "hitbox.hpp"

class Car {
public:
    // Constructor
    Car(Model& model);

    // Functions
    void update(float deltaTime, GLFWwindow* window, ExhaustSystem& exhaustSystem, std::vector<Hitbox>& environmentHitboxes, std::vector<Hitbox>& wallHitboxes);
    void draw(Shader& shader);
    void gameHit();
    void reset();


    // Getters 
    glm::vec3 getPosition() const;
    glm::vec3 getForwardDirection() const;
    float getSpeed() const;
    Hitbox getHitbox() const;

private:
    Model model; // Car model
    glm::vec3 position;
    float speed;
    float maxSpeed;
    float steeringAngle;
    float turningSpeed;
    int collisionInt;

    // Hitbox for collision detection
    Hitbox hitbox;
};

#endif // CAR_HPP