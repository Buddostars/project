#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <string>

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

#endif