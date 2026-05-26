#version 410 core

in vec3 fragPos;
in vec3 normal;
in vec2 uv;

out vec4 FragColor;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};
uniform Light lightDifuse;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	int shininess;
};
uniform Material material;


void main() {
	vec3 albedoMap = vec3(texture(material.diffuse, uv));
	vec3 specularMap = vec3(texture(material.specular, uv));

	vec3 ambient = albedoMap * lightDifuse.ambient;

	vec3 normalVector = normalize(normal);
	vec3 lightDirection = lightDifuse.position - fragPos;
	float distanceToLight = length(lightDirection);
	lightDirection = normalize(lightDirection);

	float attenuation = 1.0 / (lightDifuse.constant + lightDifuse.linear * distanceToLight + lightDifuse.quadratic * (distanceToLight * distanceToLight));

	float diffuseFactor = max(dot(normalVector, lightDirection), 0.0);
	vec3 diffuse = diffuseFactor * albedoMap * lightDifuse.diffuse;

	// Blinn-Phong in View Space
	vec3 viewDirection = normalize(-fragPos); // Camera is at (0,0,0)
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularFactor = pow(max(dot(normalVector, halfwayDirection), 0.0), material.shininess);
	vec3 specular = specularFactor * specularMap * lightDifuse.specular;

	vec3 phong = (ambient + diffuse + specular) * attenuation;
	FragColor = vec4(phong, 1.0);
}
