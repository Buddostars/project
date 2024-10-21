#ifndef MESH_H
#define MESH_H

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <string>
#include "shader.h"
#include "globals.hpp"
#include "texture.hpp"
#include "material.hpp"

class Mesh {
public:
    // Mesh data
    std::vector<Vertex> vertices;           // Ensure to use std::vector
    std::vector<unsigned int> indices;      // Ensure to use std::vector
    std::vector<Texture> textures;           // Ensure to use std::vector
    Material material;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material);
    void Draw(Shader& shader);

private:
    // Render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};  

#endif
