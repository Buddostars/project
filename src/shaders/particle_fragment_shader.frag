#version 330 core

in vec2 TexCoords;  // Texture coordinates from the vertex shader
out vec4 FragColor; // Final color of the fragment

uniform sampler2D particleTexture;  // Texture for the particle (smoke)
uniform float alpha;                // Alpha value for transparency

void main() {
    // Sample the texture (smoke texture) using the texture coordinates
    vec4 texColor = texture(particleTexture, TexCoords);

    // Apply transparency to the texture color
    FragColor = vec4(texColor.rgb, texColor.a * alpha);

    // Discard completely transparent fragments for smooth blending
    if (FragColor.a < 0.1)
        discard;
}
