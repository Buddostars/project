#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;  // Received from vertex shader

// Light and view positions
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

// Non-PBR Material
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D texture_diffuse1;  // Non-PBR texture
uniform bool usePBR;  // Flag to indicate whether to use PBR

// PBR Textures (only used if usePBR is true)
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// PBR Constants
const float PI = 3.14159265359;

void main() {
    if (usePBR) {
        // Fetch PBR textures
        vec3 albedo = texture(albedoMap, TexCoords).rgb;
        vec3 normal = texture(normalMap, TexCoords).rgb * 2.0 - 1.0;  // Convert to [-1, 1]
        float metallic = texture(metallicMap, TexCoords).r;
        float roughness = texture(roughnessMap, TexCoords).r;
        float ao = texture(aoMap, TexCoords).r;

        // PBR lighting calculations (similar to the previous version)
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfDir = normalize(lightDir + viewDir);

        float NdotL = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = albedo * lightColor * NdotL;

        float NdotH = max(dot(normal, halfDir), 0.0);
        float VdotH = max(dot(viewDir, halfDir), 0.0);

        float F0 = 0.04;
        vec3 F = F0 + (albedo - vec3(F0)) * pow(1.0 - VdotH, 5.0);

        float D = (roughness * roughness) / (PI * pow(NdotH * NdotH * (roughness * roughness - 1.0) + 1.0, 2.0));
        float G = min(1.0, min(2.0 * NdotH * NdotL / VdotH, 2.0 * NdotH * NdotL / (NdotH * NdotH)));

        vec3 specular = (F * D * G) / (4.0 * NdotL * VdotH);

        vec3 ambient = albedo * lightColor * ao;
        FragColor = vec4(ambient + diffuse + specular, 1.0);
    } else {
        // Non-PBR Lighting Model
        vec3 ambient = material.ambient * texture(texture_diffuse1, TexCoords).rgb;

        // Diffuse shading
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = material.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

        // Specular shading
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = material.specular * spec;

        FragColor = vec4(ambient + diffuse + specular, 1.0);
    }
}
