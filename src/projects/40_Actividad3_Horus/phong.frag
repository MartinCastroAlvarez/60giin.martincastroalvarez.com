#version 410 core

in vec3 fragPos;
in mat3 TBN; // NORMAL MAPPING
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

struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	vec3 direction;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};
uniform SpotLight spotLight0;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1; // NORMAL MAPPING
uniform int shininess;
uniform float normalIntensity; // NORMAL MAPPING



vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo_map, vec3 specular_map) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	// BLINN-PHONG REFLECTION MODEL
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
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

	// BLINN-PHONG REFLECTION MODEL
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = spec * specular_map * light.specular;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo_map, vec3 specular_map) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 / (light.constant +
							   light.linear * dist +
							   light.quadratic * (dist * dist));

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	// BLINN-PHONG REFLECTION MODEL
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = spec * specular_map * light.specular;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / max(epsilon, 0.00001), 0.0, 1.0);
	
	return (ambient + (diffuse * intensity) + (specular * intensity)) * attenuation;
}

void main() {
	vec3 albedo_map = vec3(texture(texture_diffuse1, uv));
	vec3 specular_map = vec3(texture(texture_specular1, uv));
	vec3 normal_map = vec3(texture(texture_normal1, uv)); // NORMAL MAPPING

	vec3 mapped_normal = normal_map * 2.0 - 1.0; // NORMAL MAPPING
	mapped_normal.xy *= normalIntensity; // NORMAL MAPPING
	vec3 norm = normalize(TBN * normalize(mapped_normal)); // NORMAL MAPPING

	vec3 viewDir = normalize(-fragPos);

	vec3 phong = calcDirectionalLight(dirLight, norm, viewDir, albedo_map, specular_map);

	phong += calcPointLight(pointLight0, norm, viewDir, fragPos, albedo_map, specular_map);
	phong += calcSpotLight(spotLight0, norm, viewDir, fragPos, albedo_map, specular_map);

	FragColor = vec4(phong, 1.0);
}