#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "shader.h"
#include "globals.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "vertex.hpp"

class Model {
public:
    Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
    }
    void draw(Shader& shader);
    std::vector<Texture> textures_loaded; 
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;
private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

#endif
