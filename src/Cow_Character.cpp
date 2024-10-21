#include "Cow_Character.h"
#include <cstdlib>                      // For random number generation
#include <glm/gtc/matrix_transform.hpp> // For rotation
#include <functional>
#include <thread>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

Cow_Character::Cow_Character(Model &model)
    : cowModel(model), position(0.0f, 0.0f, -10.0f), direction(0.0f, 0.0f, 1.0f), distanceTraveled(0.0f), moving(true),
      rotationAngle(180.0f), stopDuration(0.0f), timeStopped(0.0f), velocity(0.0f), maxSpeed(20.0f),
      acceleration(1.0f), deceleration(3.0f), totalRotationAngle(0.0f),
      hitbox(cowModel.calculateHitbox()) // define hitbox
{
    // Initialize cow position, direction, and movement state
}

void Cow_Character::moveRandomly(float deltaTime, const std::vector<Hitbox> &environmentHitboxes, const std::vector<Hitbox> &wallHitboxes)
{
    // If cow is knocked back, gradually reduce velocity
    if (glm::length2(velocity) > 0.0f)
    {                                              // Check if velocity is non-zero
        int subSteps = 20;                         // Number of sub-steps to improve collision detection
        float subDeltaTime = deltaTime / subSteps; // Time step for sub-steps

        for (int i = 0; i < subSteps; i++)
        {
            position += velocity * subDeltaTime; // Apply current velocity to position

            // Apply deceleration to reduce knockback over time
            float deceleration = 5.0f; // Rate at which the knockback fades
            velocity -= glm::normalize(velocity) * deceleration * subDeltaTime;

            // Stop when velocity is near zero
            if (glm::length2(velocity) < 0.01f)
            {
                velocity = glm::vec3(0.0f);
                break;
            }

            // Update the cow's hitbox position
            glm::vec3 boxMin = position + glm::vec3(-1.0f, 0.0f, -1.0f);
            glm::vec3 boxMax = position + glm::vec3(1.0f, 2.0f, 1.0f);
            hitbox = Hitbox(boxMin, boxMax);

            // Check for collisions with walls during sub-steps
            for (const auto &wallHitbox : wallHitboxes)
            {
                if (hitbox.isColliding(wallHitbox))
                {

                    velocity = glm::vec3(0.0f); // Stop the cow
                                                // Adjust position based on actual overlap, not fixed values
                    glm::vec3 overlap;
                    overlap.x = (hitbox.maxCorner.x - wallHitbox.minCorner.x) > (wallHitbox.maxCorner.x - hitbox.minCorner.x)
                                    ? wallHitbox.maxCorner.x - hitbox.minCorner.x
                                    : hitbox.maxCorner.x - wallHitbox.minCorner.x;
                    overlap.z = (hitbox.maxCorner.z - wallHitbox.minCorner.z) > (wallHitbox.maxCorner.z - hitbox.minCorner.z)
                                    ? wallHitbox.maxCorner.z - hitbox.minCorner.z
                                    : hitbox.maxCorner.z - wallHitbox.minCorner.z;

                    // Adjust cow's position to resolve the collision based on overlap
                    if (overlap.x < overlap.z)
                    {
                        if (direction.x > 0)
                        {
                            position.x -= overlap.x; // Move back on x-axis
                        }
                        else
                        {
                            position.x += overlap.x;
                        }
                    }
                    else
                    {
                        if (direction.z > 0)
                        {
                            position.z -= overlap.z; // Move back on z-axis
                        }
                        else
                        {
                            position.z += overlap.z;
                        }
                    }

                    std::cout << "Wall collision resolved by adjusting position!" << std::endl;
                    return; // Stop further movement after collision resolution
                }
            }
        }
        return; // Skip normal movement if in knockback
    }

    float maxDistance = 10.0f; // Move forward for 10 meters
    float accelerationMultiplier = 1.5f;
    rotationSpeed = 50.0f;

    if (isRotating)
    {
        moving = true;

        // Smoothly rotate towards the target angle
        float rotationStep = rotationSpeed * deltaTime;

        // Check if the cow needs to keep rotating or stop
        if (fabs(targetRotationAngle) > rotationStep)
        {
            // Continue rotating towards the target
            totalRotationAngle += (targetRotationAngle > 0 ? rotationStep : -rotationStep);
            targetRotationAngle -= (targetRotationAngle > 0 ? rotationStep : -rotationStep);

            // // Accelerate the cow towards max speed
            // if (speed < maxSpeed) {
            //     speed += acceleration * deltaTime * accelerationMultiplier;
            //     if (speed > maxSpeed) speed = maxSpeed;  // Cap the speed at max speed
            // }

            // // Move forward in the current direction based on the current speed
            // position += direction * speed * deltaTime;
        }
        else
        {
            // Finish the rotation, update the direction vector
            totalRotationAngle += targetRotationAngle; // Apply the remaining rotation
            targetRotationAngle = 0.0f;
            isRotating = false; // Rotation is done
            moving = true;      // Resume moving

            // Update the direction based on the new rotation angle
            float radians = glm::radians(totalRotationAngle);
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));
            direction = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f))); // Update the forward direction
        }
        // std::cout << "Rotating by: " << rotationStep << " degrees, Remaining: " << targetRotationAngle << std::endl;
    }
    if (moving)
    {
        // Accelerate the cow towards max speed
        if (speed < maxSpeed)
        {
            speed += acceleration * deltaTime * accelerationMultiplier;
            if (speed > maxSpeed)
                speed = maxSpeed; // Cap the speed at max speed
        }

        // Move forward in the current direction based on the current speed
        glm::vec3 newPosition = position + direction * speed * deltaTime;
        distanceTraveled += speed * deltaTime;

        // Check if the cow has traveled the maximum distance
        if (distanceTraveled >= maxDistance)
        {
            moving = false;                     // Stop moving
            distanceTraveled = 0.0f;            // Reset distance traveled
            stopDuration = 0.5f + (rand() % 4); // Set random stop duration between .5 and 3.5 seconds
            timeStopped = 0.0f;                 // Reset the stopped time
        }

        // Check for collisions with the environment
        Hitbox newHitbox = hitbox;
        glm::vec3 offset = newPosition - position;
        newHitbox.minCorner += offset;
        newHitbox.maxCorner += offset;

        bool collision = false;
        for (const auto &envHitbox : environmentHitboxes)
        {
            if (newHitbox.isColliding(envHitbox))
            {
                collision = true;
                speed = 0.0f; // Stop the cow
                std::cout << "Environment collision detected! Cow stopped" << std::endl;
                break;
            }
        }

        // Check for collisions with the walls
        bool wallCollision = false;
        for (const auto &wallHitbox : wallHitboxes)
        {
            if (newHitbox.isColliding(wallHitbox))
            {
                wallCollision = true;

                // Determine which axis to bounce on
                if (newHitbox.minCorner.x <= wallHitbox.maxCorner.x || newHitbox.maxCorner.x >= wallHitbox.minCorner.x)
                {
                    // Bounce on the X-axis
                    direction.x = -direction.x;

                    // clamp the cow position to stay within the wall
                    position.x = direction.x > 0 ? wallHitbox.maxCorner.x + 1.0f : wallHitbox.minCorner.x - 1.0f;

                    std::cout << "Wall collision detected! Cow bounced off" << std::endl;
                }
                if (newHitbox.minCorner.z <= wallHitbox.maxCorner.z || newHitbox.maxCorner.z >= wallHitbox.minCorner.z)
                {
                    // Bounce on the Z-axis
                    direction.z = -direction.z;
                    // clamp the cow position to stay within the wall
                    position.z = direction.z > 0 ? wallHitbox.maxCorner.z + 1.0f : wallHitbox.minCorner.z - 1.0f;

                    std::cout << "Wall collision detected! Cow bounced off" << std::endl;
                }

                break;
            }
        }

        // If no collision detected, update position; otherwise, bounce off the wall
        if (!collision && !wallCollision)
        {
            position = newPosition;
        }
        else if (wallCollision)
        {
            speed *= 0.5f; // Reduce speed when bouncing off the wall
        }
        else
        {
            speed = 0.0f; // Stop the cow
            std::cout << "Environment collision detected! Cow stopped" << std::endl;
        }
    }
    else
    {
        // Decelerate the cow smoothly when stopping
        if (speed > 0.0f)
        {
            speed -= deceleration * deltaTime;
            if (speed < 0.0f)
                speed = 0.0f; // Prevent speed from going negative
        }

        // Update how long the cow has been stopped
        timeStopped += deltaTime;

        // Check if the cow has stopped long enough
        if (timeStopped >= stopDuration)
        {
            stopAndRotate(); // Rotate and start moving again
        }
    }

    // Update the cow's hitbox position
    glm::vec3 boxMin = position + glm::vec3(-1.0f, 0.0f, -1.0f);
    glm::vec3 boxMax = position + glm::vec3(1.0f, 2.0f, 1.0f);
    hitbox = Hitbox(boxMin, boxMax);
}

void Cow_Character::stopAndRotate()
{
    // Randomly select the rotation angle (clockwise or counterclockwise)
    float randomRotationAngle = ((rand() % 2) == 0) ? 45.0f : -45.0f; // Rotate randomly by 45 degrees

    // Set the target rotation angle
    targetRotationAngle = randomRotationAngle;

    // Set the rotation flag to start rotating
    isRotating = true;

    // The cow will stop moving and rotate over time
    // std::cout << "Old direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
    // std::cout << "Target rotation angle: " << randomRotationAngle << std::endl;
}

glm::vec3 Cow_Character::getPosition()
{
    return position;
}

float Cow_Character::getTotalRotationAngle()
{
    return totalRotationAngle;
}

void Cow_Character::draw(Shader &shader)
{
    cowModel.draw(shader); // Render the cow model
}

Hitbox Cow_Character::getHitbox() const
{
    return hitbox;
}

float Cow_Character::getSpeed() const{
    return speed;
}

// Knockback logic
void Cow_Character::gameHit(glm::vec3 hitDirection, float carSpeed)
{
    // Apply knockback based on the car's direction and speed
    float knockbackMultiplier = 5.0f; // Control the force of knockback
    glm::vec3 knockbackVelocity = glm::normalize(hitDirection) * carSpeed * knockbackMultiplier;

    // Update cow's velocity to apply knockback
    velocity = knockbackVelocity;

    std::cout << "Cow hit! Knockback velocity: " << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl;
}