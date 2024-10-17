#include "environment.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <random>

// Constructor
Environment::Environment(Shader &shaderProgam)
    : shader(shaderProgam), ground("src/models/ground.obj"), tree("src/models/Tree.obj"), big_rock("src/models/big_rock.obj"), small_rock("src/models/Small_Rock.obj")
{

    // Generate object positions
    int treeCount = 50;
    treePositions = generateSpacedObjectPositions(treeCount, 90.0f, 15.0f); // Range -90 to 90, at least 5 units apart

    int bigRockCount = 35;
    bigRockPositions = generateSpacedObjectPositions(bigRockCount, 90.0f, 15.0f);

    int smallRockCount = 35;
    smallRockPositions = generateSpacedObjectPositions(smallRockCount, 90.0f, 15.0f);
}

// Render the environment models
void Environment::render(const glm::mat4 &view, const glm::mat4 &projection)
{
    // Draw the ground model
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Position of cow
    shader.setMat4("model", groundModel);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    ground.draw(shader); // Draw ground

    // Draw the tree model using fixed positions
    for (const auto &position : treePositions)
    {
        glm::mat4 treeModel = glm::mat4(1.0f);
        treeModel = glm::translate(treeModel, position);                // Use fixed position
        treeModel = glm::scale(treeModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale trees if necessary

        shader.setMat4("model", treeModel);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        tree.draw(shader); // Draw tree
    }

    // Draw the rocks
    for (const auto &position : smallRockPositions)
    {
        glm::mat4 smallRockkModel = glm::mat4(1.0f);
        smallRockkModel = glm::translate(smallRockkModel, position);                // Use fixed position
        smallRockkModel = glm::scale(smallRockkModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale trees if necessary

        shader.setMat4("model", smallRockkModel);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        big_rock.draw(shader); // Draw tree
    }

    for (const auto &position : bigRockPositions)
    {
        glm::mat4 bigRockkModel = glm::mat4(1.0f);
        bigRockkModel = glm::translate(bigRockkModel, position);                // Use fixed position
        bigRockkModel = glm::scale(bigRockkModel, glm::vec3(1.5f, 1.5f, 1.5f)); // Scale trees if necessary

        shader.setMat4("model", bigRockkModel);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        big_rock.draw(shader); // Draw tree
    }
}

// Generate pseudo-random object positions with at least 5 units of distance between them
std::vector<glm::vec3> Environment::generateSpacedObjectPositions(int count, float range, float minDistance)
{
    std::vector<glm::vec3> positions;

    // Random number generator for X and Z positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-range, range);

    while (positions.size() < count)
    {
        // Generate a new potential position
        glm::vec3 newPosition(dist(gen), 0.0f, dist(gen));

        // Check if the new position is valid (i.e., far enough from other positions)
        if (isPositionValid(newPosition, positions, minDistance))
        {
            positions.push_back(newPosition);
        }
    }

    return positions;
}

// Function to check the distance between two positions
bool Environment::isPositionValid(const glm::vec3& newPosition, const std::vector<glm::vec3>& existingPositions, float minDistance) {
    for (const auto& pos : existingPositions) {
        float distance = glm::distance(newPosition, pos);
        if (distance < minDistance) {
            return false;
        }
    }
    return true;
}
