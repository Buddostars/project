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
#include "material.hpp"
#include "vertex.hpp"
#include "hitbox.hpp"

class Model {
public:
    Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
    }
    void draw(Shader& shader, unsigned int cubemapTextureID = -1);
    std::vector<Texture> textures_loaded; 
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    // Hitbox for collision detection
    Hitbox calculateHitbox() const;
    
private:
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    Material loadMaterials(aiMaterial *mat);
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

#endif
