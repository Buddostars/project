#include "Giraffe_Character.h"
#include <cstdlib>                      // For random number generation
#include <glm/gtc/matrix_transform.hpp> // For rotation
#include <functional>
#include <thread>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp> // For rotation
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

Giraffe_Character::Giraffe_Character(Model& model)
    : giraffeModel(model), position(0.0f, 0.0f, -30.0f), direction(0.0f, 0.0f, -1.0f), distanceTraveled(0.0f), moving(true),
      rotationAngle(180.0f), stopDuration(0.0f), timeStopped(0.0f), velocity(0.0f), maxSpeed(20.0f),
      acceleration(1.0f), deceleration(3.0f), totalRotationAngle(0.0f), counter(0.0f), currentRotationAngle(0.0f),
      rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
      rotationDist(-45.0f, 45.0f),  // Set up the random distribution for rotation angles
      hitbox(giraffeModel.calculateHitbox()) {
    // Initialize giraffe position, direction, and movement state
}

Giraffe_Character::Giraffe_Character(Model& model, glm::vec3 position)
    : giraffeModel(model), position(position), direction(0.0f, 0.0f, -1.0f), distanceTraveled(0.0f), moving(true),
      rotationAngle(180.0f), stopDuration(0.0f), timeStopped(0.0f), velocity(0.0f), maxSpeed(20.0f),
      acceleration(1.0f), deceleration(3.0f), totalRotationAngle(0.0f), counter(0.0f),
      rng(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
      rotationDist(-15.0f, 15.0f),  // Set up the random distribution for rotation angles
      hitbox(giraffeModel.calculateHitbox()) {
    // Initialize giraffe position, direction, and movement state
}

// Move constructor
Giraffe_Character::Giraffe_Character(Giraffe_Character&& other) noexcept
    : giraffeModel(other.giraffeModel), 
      position(std::move(other.position)),
      // Move other necessary fields...
      velocity(std::move(other.velocity)),
      maxSpeed(other.maxSpeed),
      acceleration(other.acceleration),
      totalRotationAngle(other.totalRotationAngle) {
    // No need to move the mutex as it’s not copyable; it’s re-initialized
}

// Move assignment operator
Giraffe_Character& Giraffe_Character::operator=(Giraffe_Character&& other) noexcept {
    if (this != &other) {
        // Move the model reference and other members
        giraffeModel = other.giraffeModel;
        position = std::move(other.position);
        velocity = std::move(other.velocity);
        maxSpeed = other.maxSpeed;
        acceleration = other.acceleration;
        totalRotationAngle = other.totalRotationAngle;
        // Reinitialize the mutex instead of moving it
    }
    return *this;
}

// Safe modification of position inside moveRandomly
void Giraffe_Character::moveRandomly(float deltaTime) {
    std::lock_guard<std::mutex> lock(giraffeMutex);
    
    // // If giraffe is knocked back, gradually reduce velocity
    // if (glm::length2(velocity) > 0.0f) { // Check if velocity is non-zero
    //     position += velocity * deltaTime;  // Apply current velocity to position
    //     // Apply deceleration to reduce knockback over time
    //     float deceleration = 5.0f;  // Rate at which the knockback fades
    //     velocity -= glm::normalize(velocity) * deceleration * deltaTime;
    //     // Stop when velocity is near zero
    //     if (glm::length2(velocity) < 0.01f) {
    //         velocity = glm::vec3(0.0f);
    //     }
    //     // Update the giraffe's hitbox position
    //     glm::vec3 boxMin = position + glm::vec3(-1.0f, 0.0f, -1.0f);
    //     glm::vec3 boxMax = position + glm::vec3(1.0f, 2.0f, 1.0f);
    //     hitbox = Hitbox(boxMin, boxMax);
    //     return; // Skip normal movement if in knockback
    // }

    // If giraffe is knocked down, don't allow normal movement
    if (isKnockedDown) {
        recoverFromKnockdown(deltaTime);
        return;
    }
    
    float maxDistance = 10.0f;  // Move forward for 10 meters
    float accelerationMultiplier = 1.5f;
    rotationSpeed = 20.0f;
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
    // Use the instance-specific random generator and distribution for the rotation angle
    auto threadIdHash = std::hash<std::thread::id>{}(std::this_thread::get_id());
    rng.seed(threadIdHash);

    // Print the thread ID hash
    // std::cout << "Thread ID hash: " << threadIdHash << std::endl;

    targetRotationAngle = rotationDist(rng);
    targetRotationAngle = targetRotationAngle + ((targetRotationAngle / abs(targetRotationAngle)) * 15);
    // std::cout << "Giraffe stopped. Target Rotation Angle: " << targetRotationAngle << std::endl;
    isRotating = true;
}

// Safe access to the giraffe's position
glm::vec3 Giraffe_Character::getPosition() {
    std::lock_guard<std::mutex> lock(giraffeMutex);
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
void Giraffe_Character::gameHit(glm::vec3 hitDirection, float cowSpeed, float deltaTime, int& gameScore) {
    if (!isKnockedDown) {
        gameScore++;
        // Set the knockback velocity
        float knockbackMultiplier = 5.0f;
        glm::vec3 knockbackVelocity = glm::normalize(hitDirection) * cowSpeed * knockbackMultiplier;
        velocity = knockbackVelocity;

        // Initiate knockdown effect
        isKnockedDown = true;
        knockdownDuration = 3.0f;  // The giraffe stays knocked down for 3 seconds
        knockdownTimer = 0.0f;

        // Set the target rotation angle for knockdown (rotate along the X-axis)
        targetRotationAngle = 90.0f;  // Smoothly rotate to lay the giraffe down
        rotationSpeed = 200.0f;

        std::cout << "Giraffe knocked down! Knockback velocity: " << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl;
    }
}


void Giraffe_Character::knockdown() {
    isKnockedDown = true;
    knockdownDuration = 3.0f;  // The giraffe will stay down for 3 seconds
    knockdownTimer = 0.0f;

    // Rotate the giraffe to simulate a knockdown (rotate along the X axis)
    totalRotationAngle += 90.0f;  // Adjust this for how you want the giraffe to "lay down"
}

void Giraffe_Character::recoverFromKnockdown(float deltaTime) {
    // knockdownTimer += deltaTime;
    // if (knockdownTimer >= knockdownDuration) {
    //     // Giraffe has recovered, allow it to stand up again
    //     isKnockedDown = false;
    //     totalRotationAngle -= 90.0f;  // Reset the rotation to stand the giraffe back up
    //     velocity = glm::vec3(0.0f);   // Stop any knockback movement
    //     std::cout << "Giraffe recovered from knockdown." << std::endl;
    // } else {
    //     // The giraffe remains knocked down, you can animate it lying down here if needed
    //     std::cout << "Giraffe is knocked down." << std::endl;
    // }
}

void Giraffe_Character::update(float deltaTime) {
    std::lock_guard<std::mutex> lock(giraffeMutex);

    float rotationStep = rotationSpeed * deltaTime;

    if (isKnockedDown) {
        knockdownTimer += deltaTime;
        
        
        // Smoothly rotate towards the target angle
        float rotationStep = rotationSpeed * deltaTime;
        if (fabs(targetRotationAngle) > rotationStep) {
            totalRotationAngle += (targetRotationAngle > 0 ? rotationStep : -rotationStep);
            targetRotationAngle -= (targetRotationAngle > 0 ? rotationStep : -rotationStep);
        } else {
            totalRotationAngle += targetRotationAngle;
            targetRotationAngle = 0.0f;
        }
    }

    // Apply the current rotation to the giraffe's transformation matrix (apply rotation along **X-axis** for forward/backward fall)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(totalRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    direction = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)));
    // std::cout << rotationStep << " " << rotationSpeed << " " << deltaTime << " " << totalRotationAngle << " " << targetRotationAngle << std::endl;
}

void Giraffe_Character::reset(const glm::vec3& newPosition) {
    std::lock_guard<std::mutex> lock(giraffeMutex);

    position = newPosition;
    direction = glm::vec3(0.0f, 0.0f, -1.0f);
    distanceTraveled = 0.0f;
    moving = true;
    rotationAngle = 180.0f;
    stopDuration = 0.0f;
    timeStopped = 0.0f;
    velocity = glm::vec3(0.0f);
    speed = 0.0f;
    totalRotationAngle = 0.0f;
    isRotating = false;
    targetRotationAngle = 0.0f;
    rotationSpeed = 20.0f;  // Set to initial rotation speed
    isKnockedDown = false;
    knockdownDuration = 0.0f;
    knockdownTimer = 0.0f;
    // Reinitialize random number generator
    rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    // Recalculate hitbox
    hitbox = giraffeModel.calculateHitbox();
}
