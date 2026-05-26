#version 410 core

in vec3 fragPos;
flat in vec3 normal;

in vec4 fragPosLightSpace;
uniform sampler2D shadowMapFaceted;
uniform float shadowIntensityFaceted;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMapFaceted, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapFaceted, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapFaceted, projCoords.xy + vec2(x, y)).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
    return shadow;
}


struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position; // In VIEW SPACE
	float constant;
	float linear;
	float quadratic;
};
uniform Light lightFaceted;

uniform vec3 objectColor;

void main() {
	vec3 ambient = objectColor * lightFaceted.ambient;

	vec3 normalVector = normalize(normal);
	vec3 lightDirection = lightFaceted.position - fragPos;
	float distanceToLight = length(lightDirection);
	lightDirection = normalize(lightDirection);

	float attenuation = 1.0 / (lightFaceted.constant + lightFaceted.linear * distanceToLight + lightFaceted.quadratic * (distanceToLight * distanceToLight));

	float diffuseFactor = max(dot(normalVector, lightDirection), 0.0);
	vec3 diffuse = diffuseFactor * objectColor * lightFaceted.diffuse;

	// Blinn-Phong in View Space
	vec3 viewDirection = normalize(-fragPos); // Camera is at (0,0,0)
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularFactor = pow(max(dot(normalVector, halfwayDirection), 0.0), 64.0);
	vec3 specular = specularFactor * vec3(1.0) * lightFaceted.specular;

	float shadow = ShadowCalculation(fragPosLightSpace, lightDirection, normalVector);
	vec3 phong = (ambient + (1.0 - shadow * shadowIntensityFaceted) * (diffuse + specular)) * attenuation;
	FragColor = vec4(phong, 1.0);
}
