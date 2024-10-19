#ifndef EXHAUSTSYSTEM_HPP
#define EXHAUSTSYSTEM_HPP

#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include <random>
#include "texture_loader.h"

struct Smoke {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;
    float size;
    float alpha;  // Opacity
};

class ExhaustSystem {
public:
    std::vector<Smoke> particles;
    int maxParticles;
    glm::vec3 exhaustPosition;  // Relative position of the exhaust pipe

    ExhaustSystem(int maxParticles, glm::vec3 exhaustPosition);

    void update(float deltaTime, const glm::vec3& carPosition);
    void emitParticles(const glm::vec3& carPosition);
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    void renderQuad();  // Function to render a quad or particle texture
};

#endif  // EXHAUSTSYSTEM_HPP
