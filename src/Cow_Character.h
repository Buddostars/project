#ifndef COW_CHARACTER_H
#define COW_CHARACTER_H

#include <glm/glm.hpp>
#include "model.hpp"
#include "hitbox.hpp"

class Cow_Character {
public:
    Cow_Character(Model& model);

    glm::vec3 getPosition();  // Returns the cow's current position
    float getTotalRotationAngle();
    Hitbox getHitbox() const; // Returns the cow's hitbox for collision detection
    float getSpeed() const;
    
    void gameHit(glm::vec3 hitDirection, float carSpeed);  // Add knockback logic
    void moveRandomly(float deltaTime, const std::vector<Hitbox>& environmentHitboxes, const std::vector<Hitbox>& wallHitboxes );  // Updates the cow's position randomly based on deltaTime
    void draw(Shader& shader);  // Renders the cow model

private:
    glm::vec3 position;       // The current position of the cow
    glm::vec3 direction;      // The direction the cow is facing
    glm::vec3 velocity;
    float distanceTraveled;   // The distance the cow has traveled since the last stop
    bool moving;              // Indicates if the cow is currently moving
    bool isRotating;           // Indicates if the cow is currently rotating
    float rotationAngle;      // The angle by which the cow rotates after stopping
    float targetRotationAngle; // The target rotation angle
    float rotationSpeed;       // The speed of rotation (degrees per second)
    float totalRotationAngle;  // The total rotation angle applied to the cow

    // Velocity and movement parameters
    float speed;           // The cow's current velocity
    float maxSpeed;           // Maximum speed the cow can reach
    float acceleration;       // The rate at which the cow accelerates
    float deceleration;       // The rate at which the cow decelerates

    // Variables for stop duration and timer
    float stopDuration;       // The duration for which the cow will stop
    float timeStopped;        // How long the cow has been stopped

    void stopAndRotate();     // Function to handle stopping and rotating
    Model& cowModel;          // The cow's 3D model

    // Hitbox for collision detection
    Hitbox hitbox;
};

#endif
