#version 330 core

layout (location = 0) in vec3 aPos;      // Position of the vertex (from the quad)
layout (location = 1) in vec2 aTexCoords; // Texture coordinates

out vec2 TexCoords;  // Pass texture coordinates to the fragment shader

uniform mat4 model;  // Model matrix to scale/translate the particle
uniform mat4 view;   // View matrix (camera)
uniform mat4 projection; // Projection matrix (perspective or orthographic)

void main() {
    // Apply the model, view, and projection transformations
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pass through texture coordinates
    TexCoords = aTexCoords;
}
