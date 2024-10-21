#version 330 core

out vec4 FragColor;                          // Output color

in vec2 TexCoords;                           // Texture coordinates from vertex shader
in vec3 FragPos;                             // Fragment position from vertex shader
in vec3 Normal;                              // Normal from vertex shader
in vec3 Tangent;                             // Tangent from vertex shader
in vec3 Bitangent;                           // Bitangent from vertex shader

// PBR textures
uniform sampler2D albedoMap;                // Albedo texture
uniform sampler2D normalMap;                // Normal texture
uniform sampler2D metallicMap;              // Metallic texture
uniform sampler2D roughnessMap;             // Roughness texture
uniform sampler2D aoMap;                    // Ambient occlusion texture

// Light properties
uniform vec3 lightPos;                       // Position of the light
uniform vec3 viewPos;                        // Camera position
uniform vec3 lightColor;                     // Color of the light

const float PI = 3.14159265359;              // Define PI

void main() {
    // Fetch textures
    vec3 albedo = texture(albedoMap, TexCoords).rgb;
    vec3 normal = texture(normalMap, TexCoords).rgb * 2.0 - 1.0; // Convert to [-1, 1]
    float metallic = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao = texture(aoMap, TexCoords).r;

    // Lighting calculations
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    // Compute diffuse and specular components (simplified PBR)
    float NdotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = albedo * lightColor * NdotL;

    // Calculate specular using Cook-Torrance model
    float NdotH = max(dot(normal, halfDir), 0.0);
    float VdotH = max(dot(viewDir, halfDir), 0.0);
    
    // Fresnel-Schlick approximation
    float F0 = 0.04; // Base reflectivity for non-metallic surfaces
    vec3 F = F0 + (albedo - vec3(F0)) * pow(1.0 - VdotH, 5.0); // Fresnel term

    // D and G terms
    float D = (roughness * roughness) / (PI * pow(NdotH * NdotH * (roughness * roughness - 1.0) + 1.0, 2.0));
    float G = min(1.0, min(2.0 * NdotH * NdotL / VdotH, 2.0 * NdotH * NdotL / (NdotH * NdotH)));

    // Specular reflection
    vec3 specular = (F * D * G) / (4.0 * NdotL * VdotH);

    // Final color
    vec3 ambient = albedo * lightColor * ao; // Ambient lighting
    FragColor = vec4(ambient + diffuse + specular, 1.0); // Combine components
}
