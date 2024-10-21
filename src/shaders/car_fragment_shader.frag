#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D texture_diffuse1;
uniform samplerCube cubemapTexture;
uniform vec3 cameraPos;  // Pass the camera position to the fragment shader
uniform bool useCubemap; // Flag to determine if the cubemap is used

void main()
{
    // Sample the diffuse texture color
    vec3 diffuseColor = texture(texture_diffuse1, TexCoords).rgb;

    // Calculate reflection if cubemap is given
    vec3 reflectedColor = vec3(0.0); // Initialize to black (no reflection)
    if (useCubemap) {
        vec3 viewDir = normalize(cameraPos - WorldPos);
        vec3 reflectDir = reflect(-viewDir, normalize(Normal));
        reflectedColor = texture(cubemapTexture, reflectDir).rgb;
    }

    // Mix diffuse and reflected colors based on reflectiveness (you can adjust this blending factor)
    float reflectivity = 0.5;  // You can set this based on material properties if needed
    vec3 finalColor = mix(diffuseColor * material.diffuse, reflectedColor, reflectivity);

    FragColor = vec4(finalColor, 1.0);
}
