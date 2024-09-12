#version 330 core

out vec4 FragColor;

in vec3 FragPos;  // Fragment position from the vertex shader
in vec3 Normal;   // Normal vector from the vertex shader

// Light properties
uniform vec3 lightPos;    // Position of the light source
uniform vec3 lightColor;  // Color of the light
uniform vec3 viewPos;     // Position of the camera/viewer

void main() {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting (Lambert's cosine law)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting (Phong model)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all components
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}
