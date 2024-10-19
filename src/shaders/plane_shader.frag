#version 330 core
out vec4 FragColor;

uniform vec3 emissiveColor;

void main() {
    // Add the emissive color to simulate glowing
    FragColor = vec4(emissiveColor, 1.0);  // Green glowing effect
}
