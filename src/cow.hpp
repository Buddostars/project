#ifndef COW_HPP
#define COW_HPP

#include <glm/glm.hpp>

class Cow {
public:
    Cow();
    void update();
    void render();

private:
    glm::vec3 position;
    float speed;
};

#endif
