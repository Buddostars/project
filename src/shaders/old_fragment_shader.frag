#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 reflectedVector;
in vec3 refractedVector;



struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D texture_diffuse1;
uniform samplerCube enviroMap;
uniform bool applyReflection;

const vec3 lightDirection = normalize(vec3(0.2, -1.0, 0.3));
const float ambient = 0.3;


void main()
{   
    //float brightness = max(dot(-lightDirection, normalize(Normal)), 0.0) + ambient;
    vec3 color = texture(texture_diffuse1, TexCoords).rgb; //* brightness;

    FragColor = vec4(color * material.diffuse, 1.0);

    if(applyReflection && material.shininess > 650) {
        vec4 reflectedColour = texture(enviroMap, reflectedVector);
	    vec4 refractedColour = texture(enviroMap, refractedVector);
	    vec4 enviroColour = mix(reflectedColour, refractedColour, 0.5);
        FragColor = mix(FragColor, enviroColour, 0.8);
    }
}
