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
};
uniform Light light;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform int shininess;

uniform vec3 viewPos;

void main() {
	vec3 albedo_map = vec3(texture(texture_diffuse1, uv));
	vec3 specular_map = vec3(texture(texture_specular1, uv));

	vec3 ambient = albedo_map * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
	vec3 specular = spec * specular_map * light.specular;

	vec3 phong = ambient + diffuse + specular;
	FragColor = vec4(phong, 1.0);
}