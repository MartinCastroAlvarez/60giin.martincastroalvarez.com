#version 410 core

in vec3 fragPos;
in vec3 normal;
in vec2 uv;

out vec4 FragColor;

struct DirLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
};
uniform DirLight dirLight;

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLight0;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	int shininess;
};
uniform Material material;



vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo_map, vec3 specular_map) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	// PHONG REFLECTION MODEL (NOT BLINN-PHONG)
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo_map, vec3 specular_map) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 /  (light.constant +
								light.linear * dist +
								light.quadratic * (dist * dist));

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	// PHONG REFLECTION MODEL (NOT BLINN-PHONG)
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	return (ambient + diffuse + specular) * attenuation;
}

void main() {
	vec3 albedo_map = vec3(texture(material.diffuse, uv));
	vec3 specular_map = vec3(texture(material.specular, uv));

	vec3 norm = normalize(normal);

	vec3 viewDir = normalize(-fragPos);

	vec3 phong = calcDirectionalLight(dirLight, norm, viewDir, albedo_map, specular_map);

	phong += calcPointLight(pointLight0, norm, viewDir, fragPos, albedo_map, specular_map);

	FragColor = vec4(phong, 1.0);
}