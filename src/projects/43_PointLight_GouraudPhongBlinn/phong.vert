#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;

out vec3 gouraudColor;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position; // in View Space
	float constant;
	float linear;
	float quadratic;
};
uniform Light lightGouraud;

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 objectColor;

void main() {
    vec3 fragPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 normal = normalMat * aNormal;

    vec3 ambient = objectColor * lightGouraud.ambient;

    vec3 normalVector = normalize(normal);
    vec3 lightDirection = lightGouraud.position - fragPos;
    float distanceToLight = length(lightDirection);
    lightDirection = normalize(lightDirection);

    float attenuation = 1.0 / (lightGouraud.constant + lightGouraud.linear * distanceToLight + lightGouraud.quadratic * (distanceToLight * distanceToLight));

    float diffuseFactor = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuse = diffuseFactor * objectColor * lightGouraud.diffuse;

    // Blinn-Phong in View Space
    vec3 viewDirection = normalize(-fragPos); // Camera is at (0,0,0)
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float specularFactor = pow(max(dot(normalVector, halfwayDirection), 0.0), 64.0);
    vec3 specular = specularFactor * vec3(1.0) * lightGouraud.specular;

    gouraudColor = (ambient + diffuse + specular) * attenuation;

    gl_Position = proj * vec4(fragPos, 1.0);
}
