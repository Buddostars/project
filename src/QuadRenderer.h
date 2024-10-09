// These files are unused at the moment. Quad rendering is done in main.cpp
// ToDo if time allows: Update main.cpp to use these files instead. 

#ifndef QUAD_RENDERER_H
#define QUAD_RENDERER_H

#include <glm/glm.hpp>
#include "shader.h"

class QuadRenderer {
public:
    QuadRenderer();
    ~QuadRenderer();

    void renderQuad(float x, float y, float width, float height, glm::vec3 color);
    void setProjection(const glm::mat4& projection);

private:
    unsigned int quadVAO, quadVBO;
    Shader quadShader;
    glm::mat4 projection;

    void initQuad();
};

#endif // QUAD_RENDERER_H

