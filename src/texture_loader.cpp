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
#include "../dependencies/include/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "../src/texture_loader.h"
#include <iostream>

unsigned int loadTexture(const std::string& path) {
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip image vertically if needed
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 1)
            format = GL_RED;
        else {
            // Handle other formats if necessary
        }

        glBindTexture(GL_TEXTURE_2D, texture);

        // Load the texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Generate mipmaps if needed
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        //std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return texture;
}