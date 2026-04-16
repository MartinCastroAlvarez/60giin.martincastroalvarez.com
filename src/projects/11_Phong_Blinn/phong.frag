#version 460 core

in vec3 fragPos;
in vec3 normal;

out vec4 FragColor;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
};
uniform Light light;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int shininess;
};
uniform Material material;

uniform vec3 viewPos;

void main() {
	vec3 ambient = material.ambient * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * material.diffuse * light.diffuse;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * material.specular * light.specular;

	vec3 phong = ambient + diffuse + specular;
	FragColor = vec4(phong, 1.0);
}