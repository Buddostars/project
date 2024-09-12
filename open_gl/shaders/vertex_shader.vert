#version 330 core

layout (location = 0) in vec3 aPos;     // Position attribute
layout (location = 1) in vec3 aNormal;  // Normal attribute

out vec3 FragPos;      // Pass the fragment's position to the fragment shader
out vec3 Normal;       // Pass the normal to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));   // Calculate the world-space position
    Normal = mat3(transpose(inverse(model))) * aNormal;  // Transform normal to world space

    gl_Position = projection * view * vec4(FragPos, 1.0);  // Transform vertex to clip space
}