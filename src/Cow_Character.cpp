#include "Cow_Character.h"
#include <cstdlib>  // For random number generation
#include <glm/gtc/matrix_transform.hpp> // For rotation

Cow_Character::Cow_Character(Model& model) 
    : cowModel(model), position(0.0f, 0.0f, -10.0f), direction(0.0f, 0.0f, 1.0f), distanceTraveled(0.0f), moving(true), 
      rotationAngle(180.0f), stopDuration(0.0f), timeStopped(0.0f), velocity(0.0f), maxSpeed(1.0f), 
      acceleration(1.0f), deceleration(3.0f), totalRotationAngle(0.0f) {
    // Initialize cow position, direction, and movement state
}


void Cow_Character::moveRandomly(float deltaTime) {
    float maxDistance = 10.0f;  // Move forward for 100 meters
    float accelerationmultiplier = 1.5;

    if (moving) {
        // Accelerate the cow towards max speed
        if (velocity < maxSpeed) {
            velocity += acceleration * deltaTime * accelerationmultiplier;
            if (velocity > maxSpeed) velocity = maxSpeed;  // Cap the velocity at max speed
        }

        // Move forward in the current direction based on the current velocity
        position += direction * velocity * deltaTime;
        distanceTraveled += velocity * deltaTime;

        // Check if the cow has traveled the maximum distance
        if (distanceTraveled >= maxDistance) {
            moving = false;  // Stop moving
            distanceTraveled = 0.0f;  // Reset distance traveled
            stopDuration = 2.0f + (rand() % 4);  // Set random stop duration between 2 and 5 seconds
            timeStopped = 0.0f;  // Reset the stopped time
        }
    } else {
        // Decelerate the cow smoothly when stopping
        if (velocity > 0.0f) {
            velocity -= deceleration * deltaTime;
            if (velocity < 0.0f) velocity = 0.0f;  // Prevent velocity from going negative
        }

        // Update how long the cow has been stopped
        timeStopped += deltaTime;

        // Check if the cow has stopped long enough
        if (timeStopped >= stopDuration) {
            stopAndRotate();  // Rotate and start moving again
        }
    }
}

void Cow_Character::stopAndRotate() {
    // Randomly rotate clockwise or counterclockwise by a random angle (e.g., 45 degrees or -45 degrees)
    float rotationAngle = ((rand() % 2) == 0) ? 45.0f : -45.0f;  // Rotate randomly either way

    std::cout << "Old direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
    std::cout << "Rotation angle: " << rotationAngle << std::endl;

    // Convert the angle to radians
    float radians = glm::radians(rotationAngle);

    // Rotate the current direction vector using a rotation matrix around the Y-axis (which represents up)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));

    // Apply the rotation to the direction vector
    direction = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f)));

    // Accumulate the rotation angle
    totalRotationAngle += rotationAngle;

    std::cout << "New direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;

    // After rotating, the cow starts moving again
    moving = true;
}



glm::vec3 Cow_Character::getPosition() {
    return position;
}

void Cow_Character::draw(Shader& shader) {
    cowModel.draw(shader);  // Render the cow model
}
