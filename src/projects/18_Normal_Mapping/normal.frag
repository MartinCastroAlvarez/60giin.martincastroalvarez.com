#version 460 core

in vec2 uv;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

out vec4 FragColor;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
};
uniform Light light;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	int shininess;
};
uniform Material material;

void main() {
	vec3 albedo_map = vec3(texture(material.diffuse, uv));
	vec3 specular_map = vec3(texture(material.specular, uv));
	vec3 normal_map = vec3(texture(material.normal, uv));

	vec3 ambient = albedo_map * light.ambient;

	vec3 norm = normalize(normal_map * 2.0 - 1.0);
	vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	vec3 phong = ambient + diffuse + specular;
	FragColor = vec4(phong, 1.0);
}