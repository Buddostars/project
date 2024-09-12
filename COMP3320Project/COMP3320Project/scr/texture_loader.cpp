// texture_loader.cpp
// This file implements the function to load textures using the third party library stb_image.h


// In main.cpp, texture_loader to be used as follows to load textures for different object. 
//#include "texture_loader.h"
//
//int main() {
//    // Initialize OpenGL context, window, etc.
//
//    // Load texture
//    unsigned int texture = loadTexture("path_to_image.jpg");
//
//    // Use the texture in your rendering loop
//
//    return 0;
//}



#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"
#include "texture_loader.h"
#include <iostream>

unsigned int loadTexture(const std::string& path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    return texture;
}
