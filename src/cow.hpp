// cow.hpp
#ifndef COW_HPP
#define COW_HPP

#include "vertex.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <vector>
#include <glm/glm.hpp>

void loadCowModel(const std::string& path);
void setupCowMesh();
void drawCow();
void processCowNode(aiNode* node, const aiScene* scene);
void processCowMesh(aiMesh* mesh, const aiScene* scene);

#endif
