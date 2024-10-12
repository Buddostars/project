// These files are unused at the moment. Quad rendering is done in main.cpp
// ToDo if time allows: Update main.cpp to use these files instead. 
#include "QuadRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

QuadRenderer::QuadRenderer()
    : quadShader("src/shaders/quad_shader.vert", "src/shaders/quad_shader.frag"), projection(glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f))
{
    initQuad();

    std::cout << "QuadRenderer initialised." << std::endl;
}

QuadRenderer::~QuadRenderer() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void QuadRenderer::initQuad() {
    float vertices[] = {
        // Positions   // Colors
        0.0f, 1.0f,    1.0f, 1.0f, 1.0f, // Top-left
        0.0f, 0.0f,    1.0f, 1.0f, 1.0f, // Bottom-left
        1.0f, 0.0f,    1.0f, 1.0f, 1.0f, // Bottom-right

        0.0f, 1.0f,    1.0f, 1.0f, 1.0f, // Top-left
        1.0f, 0.0f,    1.0f, 1.0f, 1.0f, // Bottom-right
        1.0f, 1.0f,    1.0f, 1.0f, 1.0f  // Top-right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error in initQuad(): " << error << std::endl;
    }

}

void QuadRenderer::setProjection(const glm::mat4& proj) {
    projection = proj;
}

void QuadRenderer::renderQuad(float x, float y, float width, float height, glm::vec3 color) {
    quadShader.use();
    quadShader.setMat4("projection", projection);

    // Model matrix to position and scale the quad
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    quadShader.setMat4("model", model);

    // Set the quad color
    quadShader.setVec3("quadColor", color);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
