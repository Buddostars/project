#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D texture_diffuse1;

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    FragColor = vec4(color * material.diffuse, 1.0);  // Adjust this line as needed for specular, ambient, etc.
}