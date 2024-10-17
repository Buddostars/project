#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <vector>
#include <glm/glm.hpp>

#include "shader.h"
#include "model.hpp"

class Environment {
public:
    // Constructor
    Environment(Shader& shader);

    // Load models
    void loadModels();

    // Render the environment models
    void render(const glm::mat4& view, const glm::mat4& projection);


private:
    // Shaders
    Shader& shader;

    // Models
    Model ground;
    Model tree;
    Model big_rock;
    Model small_rock;

    // Model positions
    std::vector<glm::vec3> treePositions;
    std::vector<glm::vec3> bigRockPositions;
    std::vector<glm::vec3> smallRockPositions;

    // Generate object positions
    std::vector<glm::vec3> generateSpacedObjectPositions(int count, float range, float minDistance);

    bool isPositionValid(const glm::vec3& newPosition, const std::vector<glm::vec3>& existingPositions, float minDistance);
};


#endif // ENVIRONMENT_HPP