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
uniform Light light;

uniform vec3 objectColor;

void main() {
	vec3 ambient = objectColor * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * objectColor * light.diffuse;

	// Blinn-Phong in View Space
	vec3 viewDir = normalize(-fragPos); // Camera is at (0,0,0)
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * vec3(1.0) * light.specular;

	vec3 phong = (ambient + diffuse + specular) * attenuation;
	FragColor = vec4(phong, 1.0);
}
