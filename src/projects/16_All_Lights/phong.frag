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
#define NUMBER_POINT_LIGHTS 2
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];

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
#define NUMBER_SPOT_LIGHTS 2
uniform SpotLight spotLights[NUMBER_SPOT_LIGHTS];

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	int shininess;
};
uniform Material material;

uniform vec3 viewPos;

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo_map, vec3 specular_map) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
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

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo_map, vec3 specular_map) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 /  (light.constant +
								light.linear * dist +
								light.quadratic * (dist * dist));

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	return (ambient + (diffuse * intensity) + (specular * intensity)) * attenuation;
}

void main() {
	vec3 albedo_map = vec3(texture(material.diffuse, uv));
	vec3 specular_map = vec3(texture(material.specular, uv));

	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 phong = calcDirectionalLight(dirLight, norm, viewDir, albedo_map, specular_map);

	for (int i = 0; i < NUMBER_POINT_LIGHTS; ++i) {
		phong += calcPointLight(pointLights[i], norm, viewDir, fragPos, albedo_map, specular_map);
		phong += vec3(0.0);
	}

	for (int i = 0; i < NUMBER_SPOT_LIGHTS; ++i) {
		phong += calcSpotLight(spotLights[i], norm, viewDir, fragPos, albedo_map, specular_map);
	}

	FragColor = vec4(phong, 1.0);
}