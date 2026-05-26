#version 410 core

in vec3 fragPos;
in vec3 normal;

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
uniform Light lightPhongBlinn;

uniform vec3 objectColor;

void main() {
	vec3 ambient = objectColor * lightPhongBlinn.ambient;

	vec3 normalVector = normalize(normal);
	vec3 lightDirection = lightPhongBlinn.position - fragPos;
	float distanceToLight = length(lightDirection);
	lightDirection = normalize(lightDirection);

	float attenuation = 1.0 / (lightPhongBlinn.constant + lightPhongBlinn.linear * distanceToLight + lightPhongBlinn.quadratic * (distanceToLight * distanceToLight));

	float diffuseFactor = max(dot(normalVector, lightDirection), 0.0);
	vec3 diffuse = diffuseFactor * objectColor * lightPhongBlinn.diffuse;

	// Blinn-Phong in View Space
	vec3 viewDirection = normalize(-fragPos); // Camera is at (0,0,0)
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	float specularFactor = pow(max(dot(normalVector, halfwayDirection), 0.0), 64.0);
	vec3 specular = specularFactor * vec3(1.0) * lightPhongBlinn.specular;

	vec3 phong = (ambient + diffuse + specular) * attenuation;
	FragColor = vec4(phong, 1.0);
}
