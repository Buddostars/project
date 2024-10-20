#include "car.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


Car::Car(Model& carModel) 
    :model(carModel), position(0.0f, 0.0f, 0.0f), speed(0.0f), maxSpeed(50.0f), steeringAngle(0.0f), turningSpeed(90.0f),
    hitbox(carModel.calculateHitbox()) 
    {}
    

void Car::update(float deltaTime, GLFWwindow* window, ExhaustSystem& exhaustSystem, std::vector<Hitbox>& environmentHitboxes) {
    static float deceleration = 15.0f;   // Deceleration rate when W key is released
    static float brakeMultiplier = 40.0f; // Braking deceleration when S key is pressed
    float tau = 5.0f;                    // Time constant for acceleration (adjust this for acceleration speed)

    float maxReverseSpeed = -40.0f;      // Maximum reverse speed
    bool isTurning = false;              // Check if the car is turning
    bool forward = false;
    bool reverse = false;
    bool braking = false;                // Check if braking is applied
    float accelerationMultiplier = 6.0f;  // Multiplier for forward acceleration

    glm::vec3 newPosition = position;  // Predict the new position based on current speed

    // Extract the car's forward direction from the transformation matrix
    glm::vec3 forwardDirection = getForwardDirection();

    // Update car position based on direction and speed
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Car is already moving forward, keep the current speed and apply exponential acceleration
        float currentSpeed = speed;

        // Apply exponential acceleration starting from the current speed
        speed = maxSpeed - (maxSpeed - currentSpeed) * exp(-deltaTime / tau);

        forward = true;
        newPosition -= forwardDirection * speed * deltaTime;
    } 
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (speed > 0.0f) {
            // Car is moving forward, so apply braking first
            forward = true;
            braking = true;
            speed -= brakeMultiplier * deltaTime;
            if (speed < 0.0f) speed = 0.0f;  // Prevent overshooting into reverse while braking
        } 
        else {
            // Car has stopped or is already reversing, apply exponential reverse acceleration
            float currentSpeed = speed;

            // Apply exponential acceleration toward negative maxReverseSpeed
            speed = maxReverseSpeed - (maxReverseSpeed - currentSpeed) * exp(-deltaTime / tau);

            reverse = true;
        }

        // Move the car backward while reversing
        newPosition -= forwardDirection * speed * deltaTime;  // Reverse moves car forward (since speed is negative)
    } 
    else {
        // Gradually decelerate when no key is pressed
        if (speed > 0.0f) {
            forward = true;
            // Reduce speed gradually, simulating momentum (sliding)
            speed -= deceleration * deltaTime;
            if (speed < 0.0f) speed = 0.0f;  // Ensure the speed does not go negative
        } else if (speed < 0.0f) {
            reverse = true;
            // Reduce reverse speed gradually (deceleration for reverse)
            speed += deceleration * deltaTime;
            if (speed > 0.0f) speed = 0.0f;  // Ensure reverse speed doesn't cross into positive
        }

        // Apply momentum to continue moving the car forward or backward based on speed
        newPosition -= forwardDirection * speed * deltaTime;
    }

    // Handle car turning (only allow turning while moving forward or reverse)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (forward || reverse)) {
        if (forward) {
            steeringAngle += turningSpeed * deltaTime;  // Move left when moving forward
        } else if (reverse) {
            steeringAngle -= turningSpeed * deltaTime;  // Move right when moving in reverse
        }
        isTurning = true;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (forward || reverse)) {
        if (forward) {
            steeringAngle -= turningSpeed * deltaTime;  // Move right when moving forward
        } else if (reverse) {
            steeringAngle += turningSpeed * deltaTime;  // Move left when moving in reverse
        }
        isTurning = true;
    }

    // Update the particle system (smoke emission)
    exhaustSystem.update(deltaTime, position);  // The exhaust position is relative to the car's position
    
    // Check for collisions with the environment
    bool collision = false;
    Hitbox newHitbox = hitbox;
    glm::vec3 offset = newPosition - position;
    newHitbox.minCorner += offset;
    newHitbox.maxCorner += offset;

    for (const auto& envHitbox : environmentHitboxes) {
        if (newHitbox.isColliding(envHitbox)) {
            collision = true;
            break;
        }
    }
    
    // If there is a collision, speed is set to 0
    if (!collision) {
        position = newPosition;
        std::cout << "No collision detected!" << std::endl;
    } else {
        
        speed = 0.0f;
        std::cout << "Collision detected!" << std::endl;
    }



    // Update the car's hitbox position
    glm::vec3 boxMin = position + glm::vec3(-1.0f, 0.0f, -1.0f);
    glm::vec3 boxMax = position + glm::vec3(1.0f, 2.0f, 1.0f);
    hitbox = Hitbox(boxMin, boxMax);
}






void Car::draw(Shader& shader) {
    glm::mat4 carModelMatrix = glm::mat4(1.0f);
    carModelMatrix = glm::translate(carModelMatrix, position); // Position of car

    // Rotate the car 180 degrees along y axis to flip then apply the steering angle
    carModelMatrix = glm::rotate(carModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    carModelMatrix = glm::rotate(carModelMatrix, glm::radians(steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    carModelMatrix = glm::scale(carModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale car if necessary

    shader.setMat4("model", carModelMatrix);
    model.draw(shader);
}

glm::vec3 Car::getPosition() const {
    return position;
}

glm::vec3 Car::getForwardDirection() const {
    return glm::vec3(sin(glm::radians(steeringAngle)), 0.0f, cos(glm::radians(steeringAngle)));
}

float Car::getSpeed() const{
    return speed;
}

Hitbox Car::getHitbox() const {
    return hitbox;
}

void Car::gameHit() {
    std::cout << "Car and cow collided!" << std::endl;
    speed = -10.0f;
}