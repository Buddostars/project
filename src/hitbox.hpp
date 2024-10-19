#ifndef HITBOX_HPP
#define HITBOX_HPP

#include <glm/glm.hpp>

class Hitbox {
public:
    glm::vec3 minCorner, maxCorner;

    //Constructor
    Hitbox(glm::vec3 minC = glm::vec3(0.0f), glm::vec3 maxC = glm::vec3(0.0f));

    //Method to check if two hitboxes are colliding
    bool isColliding(const Hitbox& other) const;
};

#endif // HITBOX_HPP
    