#ifndef GIRAFFE_CHARACTER_H
#define GIRAFFE_CHARACTER_H

#include <glm/glm.hpp>
#include "model.hpp"
#include "hitbox.hpp"

class Giraffe_Character {
public:
    Giraffe_Character(Model& model);

    void moveRandomly(float deltaTime);  // Updates the giraffe's position randomly based on deltaTime
    glm::vec3 getPosition();  // Returns the giraffe's current position
    float getTotalRotationAngle();
    void draw(Shader& shader);  // Renders the giraffe model
    Hitbox getHitbox() const;  // Returns the giraffe's hitbox for collision detection
    void gameHit(glm::vec3 hitDirection, float carSpeed);  // Add knockback logic

private:
    glm::vec3 position;       // The current position of the giraffe
    glm::vec3 direction;      // The direction the giraffe is facing
    glm::vec3 velocity;
    float distanceTraveled;   // The distance the giraffe has traveled since the last stop
    bool moving;              // Indicates if the giraffe is currently moving
    bool isRotating;          // Indicates if the giraffe is currently rotating
    float rotationAngle;      // The angle by which the giraffe rotates after stopping
    float targetRotationAngle; // The target rotation angle
    float rotationSpeed;      // The speed of rotation (degrees per second)
    float totalRotationAngle; // The total rotation angle applied to the giraffe

    // Velocity and movement parameters
    float speed;              // The giraffe's current velocity
    float maxSpeed;           // Maximum speed the giraffe can reach
    float acceleration;       // The rate at which the giraffe accelerates
    float deceleration;       // The rate at which the giraffe decelerates

    // Variables for stop duration and timer
    float stopDuration;       // The duration for which the giraffe will stop
    float timeStopped;        // How long the giraffe has been stopped
    float counter;
    
    void stopAndRotate();     // Function to handle stopping and rotating
    Model& giraffeModel;      // The giraffe's 3D model

    // Hitbox for collision detection
    Hitbox hitbox;
};

#endif
