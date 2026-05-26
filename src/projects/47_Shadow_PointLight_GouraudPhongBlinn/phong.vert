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

out vec4 fragPosLightSpace;
uniform mat4 lightSpaceMatrixGouraud;
uniform sampler2D shadowMapGouraud;
uniform float shadowIntensityGouraud;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMapGouraud, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapGouraud, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapGouraud, projCoords.xy + vec2(x, y)).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}
uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 objectColor;

void main() {
    vec3 fragPosWorld = vec3(model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrixGouraud * vec4(fragPosWorld, 1.0);
    vec3 fragPos = vec3(view * vec4(fragPosWorld, 1.0));
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

    float shadow = ShadowCalculation(fragPosLightSpace, lightDirection, normalVector);
    gouraudColor = (ambient + (1.0 - shadow * shadowIntensityGouraud) * (diffuse + specular)) * attenuation;

    gl_Position = proj * vec4(fragPos, 1.0);
}
