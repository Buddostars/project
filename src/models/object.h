#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>    // C++ importer interface 
#include <assimp/scene.h>         // Output data structure
#include <assimp/postprocess.h>   // Post processing flags

#include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
#include <glm/gtc/matrix_transform.hpp> // For transformations
#include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers

#include "../shaders/shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Object {
public:
    // Constructor that take model from path, then initialize position and scale of object
    Object(const std::string& modelPath, const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
    ~Object();

    void Draw(Shader& shaderProgram) const;  // Marking as const if not modifying state
        // Getter for position
    glm::vec3 getPosition() const;

    // Getter for scale
    glm::vec3 getScale() const;
private:
    bool LoadModel(const std::string& modelPath);
    void SetupMesh();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO = 0, VBO = 0, EBO = 0;

    glm::vec3 position; // Store position
    glm::vec3 scale;    // Store scale
};

#endif // OBJECT_H
