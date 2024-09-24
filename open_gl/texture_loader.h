// texture_loader.h
// Defines a function to load textures

#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <GL/glew.h>
#include <string>

// No need to define STB_IMAGE_IMPLEMENTATION here
#include "../third_party/stb_image.h"

unsigned int loadTexture(const std::string& path);

#endif


