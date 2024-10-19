#include "hitbox.hpp"

// Constructor
Hitbox::Hitbox(glm::vec3 minC, glm::vec3 maxC) 
    : minCorner(minC), maxCorner(maxC) {}


// Method to check if two hitboxes are colliding
bool Hitbox::isColliding(const Hitbox& other) const {
    // Check for overlap along all three axes
    bool xOverlap = maxCorner.x >= other.minCorner.x && minCorner.x <= other.maxCorner.x;
    bool yOverlap = maxCorner.y >= other.minCorner.y && minCorner.y <= other.maxCorner.y;
    bool zOverlap = maxCorner.z >= other.minCorner.z && minCorner.z <= other.maxCorner.z;

    // If all three axes overlap, the hitboxes are colliding
    return xOverlap && yOverlap && zOverlap;
}