#ifndef CAR_HPP
#define CAR_HPP

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


class Car {
public:
    Car();
    void update(GLFWwindow* window);  // Update car position based on input
    void render();  // Render the car model
    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    float speed;
};

#endif
