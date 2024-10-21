#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 reflectedVector;
out vec3 refractedVector;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

void main()
{
    TexCoords = aTexCoords;
    Normal = aNormal;
    vec4 worldPosition = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPosition;

    vec3 unitNormal = normalize(aNormal);
	
	vec3 viewVector = normalize(worldPosition.xyz - viewPos);
	reflectedVector = reflect(viewVector, unitNormal);
	refractedVector = refract(viewVector, unitNormal, 1.0/1.1);
}