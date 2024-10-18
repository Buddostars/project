#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Receive the texture coordinates from the vertex shader

uniform sampler2D texture_diffuse; // Uniform for the texture

void main() {
    FragColor = texture(texture_diffuse, TexCoords); // Sample the texture
}
