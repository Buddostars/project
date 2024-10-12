#version 330 core
layout(location = 0) in vec2 aPos; // Position attribute (2D)
layout(location = 1) in vec3 aColor; // Color attribute

out vec3 ourColor; // Pass color to fragment shader

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    ourColor = aColor;
}
