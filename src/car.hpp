#ifndef CAR_HPP
#define CAR_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <vector>
#include <assimp/Importer.hpp>    // C++ importer interface 
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


class Car {
public:
    Car();

    void update(GLFWwindow* window);  // Update car position based on input
    void render();  // Render the car model
    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    float speed;
    glm::vec3 color;
    unsigned int VAO, VBO, EBO;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void setupMesh();
};

#endif
