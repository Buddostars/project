#version 330 core

layout(location = 0) in vec3 aPos;   // Vertex position
layout(location = 1) in vec3 aNormal; // Vertex normal
layout(location = 2) in vec2 aTexCoord; // Texture coordinates

out vec3 FragPos;   // Pass to fragment shader
out vec3 Normal;    // Pass to fragment shader
out vec2 TexCoord;  // Pass to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
