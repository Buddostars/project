#include "cow.hpp"
#include <cstdlib>  // For random movement

Cow::Cow() : position(glm::vec3(25.0f, 0.0f, 10.0f)), speed(2.0f) {}

void Cow::update() {
    // Randomly move the cow along the XZ plane
    float randomMove = (rand() % 3 - 1) * speed * 0.01f;
    position.x += randomMove;
    position.z += randomMove;
}

void Cow::render() {
    // Render the cow model here
}
