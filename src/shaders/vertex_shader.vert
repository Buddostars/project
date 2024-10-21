#version 330 core

layout(location = 0) in vec3 aPos;          // Vertex position
layout(location = 1) in vec3 aNormal;       // Vertex normal
layout(location = 2) in vec2 aTexCoords;    // Texture coordinates
layout(location = 3) in vec3 aTangent;      // Vertex tangent
layout(location = 4) in vec3 aBitangent;    // Vertex bitangent

out vec2 TexCoords;                          // Pass texture coordinates to fragment shader
out vec3 FragPos;                            // Pass fragment position to fragment shader
out vec3 Normal;                             // Pass normal to fragment shader
out vec3 Tangent;                            // Pass tangent to fragment shader
out vec3 Bitangent;                          // Pass bitangent to fragment shader

uniform mat4 model;                          // Model matrix
uniform mat4 view;                           // View matrix
uniform mat4 projection;                     // Projection matrix

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // Calculate fragment position in world space
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal to world space
    Tangent = mat3(transpose(inverse(model))) * aTangent; // Transform tangent to world space
    Bitangent = mat3(transpose(inverse(model))) * aBitangent; // Transform bitangent to world space
    TexCoords = aTexCoords;                  // Pass texture coordinates to fragment shader

    gl_Position = projection * view * vec4(FragPos, 1.0); // Calculate final vertex position
}
