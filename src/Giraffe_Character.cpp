#include "Giraffe_Character.h"
#include <cstdlib>  // For random number generation
#include <glm/gtc/matrix_transform.hpp> // For rotation
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

Giraffe_Character::Giraffe_Character(Model& model)
    : giraffeModel(model), position(0.0f, 0.0f, -30.0f), direction(0.0f, 0.0f, -1.0f), distanceTraveled(0.0f), moving(true),
      rotationAngle(180.0f), stopDuration(0.0f), timeStopped(0.0f), velocity(0.0f), maxSpeed(20.0f),
      acceleration(1.0f), deceleration(3.0f), totalRotationAngle(0.0f), counter(0.0f),
      hitbox(giraffeModel.calculateHitbox()) {
    // Initialize giraffe position, direction, and movement state
}

Giraffe_Character::Giraffe_Character(Model& model, glm::vec3 position)
    : giraffeModel(model), position(position), direction(0.0f, 0.0f, -1.0f), distanceTraveled(0.0f), moving(true),
      rotationAngle(180.0f), stopDuration(0.0f), timeStopped(0.0f), velocity(0.0f), maxSpeed(20.0f),
      acceleration(1.0f), deceleration(3.0f), totalRotationAngle(0.0f), counter(0.0f),
      hitbox(giraffeModel.calculateHitbox()) {
    // Initialize giraffe position, direction, and movement state
}

void Giraffe_Character::moveRandomly(float deltaTime) {
    // If giraffe is knocked back, gradually reduce velocity
    if (glm::length2(velocity) > 0.0f) { // Check if velocity is non-zero
        position += velocity * deltaTime;  // Apply current velocity to position

        // Apply deceleration to reduce knockback over time
        float deceleration = 5.0f;  // Rate at which the knockback fades
        velocity -= glm::normalize(velocity) * deceleration * deltaTime;

        // Stop when velocity is near zero
        if (glm::length2(velocity) < 0.01f) {
            velocity = glm::vec3(0.0f);
        }

        // Update the giraffe's hitbox position
        glm::vec3 boxMin = position + glm::vec3(-1.0f, 0.0f, -1.0f);
        glm::vec3 boxMax = position + glm::vec3(1.0f, 2.0f, 1.0f);
        hitbox = Hitbox(boxMin, boxMax);

        return; // Skip normal movement if in knockback
    }

    float maxDistance = 10.0f;  // Move forward for 10 meters
    float accelerationMultiplier = 1.5f;
    rotationSpeed = 50.0f;

    if (isRotating) {
        moving = true;

        // Smoothly rotate towards the target angle
        float rotationStep = rotationSpeed * deltaTime;

        if (fabs(targetRotationAngle) > rotationStep) {
            totalRotationAngle += (targetRotationAngle > 0 ? rotationStep : -rotationStep);
            targetRotationAngle -= (targetRotationAngle > 0 ? rotationStep : -rotationStep);
        } else {
            totalRotationAngle += targetRotationAngle;
            targetRotationAngle = 0.0f;
            isRotating = false;
            moving = true;

            float radians = glm::radians(totalRotationAngle);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
            direction = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
        }
    }

    if (moving) {
        // if (speed < maxSpeed) {
        //     speed += acceleration * deltaTime * accelerationMultiplier;
        //     if (speed > maxSpeed) speed = maxSpeed;
        // }

        // position += direction * speed * deltaTime;
        // distanceTraveled += speed * deltaTime;

        // if (distanceTraveled >= maxDistance) {
            moving = false;
            distanceTraveled = 0.0f;
            stopDuration = 0.5f + (rand() % 4);
            timeStopped = 0.0f;
        // }
    } else {
        // if (speed > 0.0f) {
        //     speed -= deceleration * deltaTime;
        //     if (speed < 0.0f) speed = 0.0f;
        // }

        timeStopped += deltaTime;

        if (timeStopped >= stopDuration) {
            stopAndRotate();
        }
    }

    glm::vec3 boxMin = position + glm::vec3(-1.0f, 0.0f, -1.0f);
    glm::vec3 boxMax = position + glm::vec3(1.0f, 2.0f, 1.0f);
    hitbox = Hitbox(boxMin, boxMax);
}

void Giraffe_Character::stopAndRotate() {
    float randomRotationAngle = ((rand() % 2) == 0) ? 45.0f : -45.0f;
    targetRotationAngle = randomRotationAngle;
    isRotating = true;
}

glm::vec3 Giraffe_Character::getPosition() {
    return position;
}

float Giraffe_Character::getTotalRotationAngle() {
    return totalRotationAngle;
}

void Giraffe_Character::draw(Shader& shader) {
    giraffeModel.draw(shader);  // Render the giraffe model
}

Hitbox Giraffe_Character::getHitbox() const {
    return hitbox;
}

// Knockback logic
void Giraffe_Character::gameHit(glm::vec3 hitDirection, float carSpeed) {
    float knockbackMultiplier = 5.0f;
    glm::vec3 knockbackVelocity = glm::normalize(hitDirection) * carSpeed * knockbackMultiplier;
    velocity = knockbackVelocity;

    std::cout << "Giraffe hit! Knockback velocity: " << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl;
}
