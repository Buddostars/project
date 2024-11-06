#include <glad/glad.h>
#include "ExhaustSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "texture_loader.h"

// Constructor
ExhaustSystem::ExhaustSystem(int maxParticles, glm::vec3 exhaustPosition)
    : maxParticles(maxParticles), exhaustPosition(exhaustPosition) {
    particles.reserve(maxParticles);
}

// Update the particle system
void ExhaustSystem::update(float deltaTime, const glm::vec3& carPosition) {
    // Emit new particles
    emitParticles(carPosition);

    // Update existing particles
    for (auto& particle : particles) {
        particle.position += particle.velocity * deltaTime;
        particle.lifetime -= deltaTime;
        particle.alpha -= deltaTime * 0.5f;  // Fade out over time
        particle.size += deltaTime * 0.2f;   // Particles grow as they age

        if (particle.lifetime <= 0.0f || particle.alpha <= 0.0f) {
            particle = particles.back();
            particles.pop_back();  // Remove dead particles
        }
    }
}

// Emit new particles from the exhaust
void ExhaustSystem::emitParticles(const glm::vec3& carPosition) {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> randomFloat(-0.1f, 0.1f);

    if (particles.size() < maxParticles) {
        Smoke particle;
        particle.position = carPosition + exhaustPosition;  // Emit from the exhaust pipe
        particle.velocity = glm::vec3(randomFloat(generator), 0.2f, randomFloat(generator));  // Random upward velocity
        particle.lifetime = 2.0f;  // Smoke lasts 2 seconds
        particle.size = 0.1f;      // Start with small size
        particle.alpha = 0.5f;     // Fully opaque at first

        particles.push_back(particle);
    }
}

// Render the particles
void ExhaustSystem::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    unsigned int smokeTextureID = loadTexture("src/smoke-img_trans.png");

    // Enable blending and disable depth writing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Enable transparency
    glDepthMask(GL_FALSE);  // Disable depth writing for transparent particles

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, smokeTextureID);
    shader.setInt("particleTexture", 0);  // Set the texture unit 0

    for (const auto& particle : particles) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, particle.position);
        model = glm::scale(model, glm::vec3(particle.size));  // Scale particle based on its size

        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setFloat("alpha", particle.alpha);  // Set transparency

        renderQuad();  // Render particle as a quad
    }

    // Restore depth writing
    glDepthMask(GL_TRUE);
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);  // Disable blending after rendering
}



// Render the particle quad (dummy function)
void ExhaustSystem::renderQuad() {
    // Vertex data for a simple quad (2D plane)
    static float quadVertices[] = {
        // Positions      // Texture Coords
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  // Top-left
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f   // Top-right
    };

    static unsigned int quadVAO = 0, quadVBO;

    // Generate the VAO/VBO only once
    if (quadVAO == 0) {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        
        // Bind VAO and VBO
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        
        // Fill VBO with vertex data
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);  // Position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));  // Texture Coords
        glEnableVertexAttribArray(1);
    }

    // Bind VAO and draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Unbind VAO after rendering
    glBindVertexArray(0);
}