#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <stb_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

class Cubemap {

public:
    Cubemap(std::vector<std::string> faces);
    void draw(Shader& shader);
    unsigned int getTextureID();

private:
    unsigned int loadCubemap(std::vector<std::string> faces);
    unsigned int textureID;
};

#endif // CUBEMAP_HPP