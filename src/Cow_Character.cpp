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
    float maxDistance = 5.0f;  // Move forward for 10 meters
    float accelerationMultiplier = 1.5f;
    rotationSpeed = 50.0f;

    if (isRotating) {
        moving = true;

        // Smoothly rotate towards the target angle
        float rotationStep = rotationSpeed * deltaTime;

        // Check if the cow needs to keep rotating or stop
        if (fabs(targetRotationAngle) > rotationStep) {
            // Continue rotating towards the target
            totalRotationAngle += (targetRotationAngle > 0 ? rotationStep : -rotationStep);
            targetRotationAngle -= (targetRotationAngle > 0 ? rotationStep : -rotationStep);

            // // Accelerate the cow towards max speed
            // if (velocity < maxSpeed) {
            //     velocity += acceleration * deltaTime * accelerationMultiplier;
            //     if (velocity > maxSpeed) velocity = maxSpeed;  // Cap the velocity at max speed
            // }

            // // Move forward in the current direction based on the current velocity
            // position += direction * velocity * deltaTime;
        } else {
            // Finish the rotation, update the direction vector
            totalRotationAngle += targetRotationAngle;  // Apply the remaining rotation
            targetRotationAngle = 0.0f;
            isRotating = false;  // Rotation is done
            moving = true;  // Resume moving

            // Update the direction based on the new rotation angle
            float radians = glm::radians(totalRotationAngle);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
            direction = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));  // Update the forward direction
        }
        std::cout << "Rotating by: " << rotationStep << " degrees, Remaining: " << targetRotationAngle << std::endl;


    }
    if (moving) {
        // Accelerate the cow towards max speed
        if (velocity < maxSpeed) {
            velocity += acceleration * deltaTime * accelerationMultiplier;
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
    // Randomly select the rotation angle (clockwise or counterclockwise)
    float randomRotationAngle = ((rand() % 2) == 0) ? 45.0f : -45.0f;  // Rotate randomly by 45 degrees

    // Set the target rotation angle
    targetRotationAngle = randomRotationAngle;

    // Set the rotation flag to start rotating
    isRotating = true;

    // The cow will stop moving and rotate over time
    std::cout << "Old direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
    std::cout << "Target rotation angle: " << randomRotationAngle << std::endl;
}


glm::vec3 Cow_Character::getPosition() {
    return position;
}

float Cow_Character::getTotalRotationAngle() {
    return totalRotationAngle;
}

void Cow_Character::draw(Shader& shader) {
    cowModel.draw(shader);  // Render the cow model
}
