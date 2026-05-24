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
	vec3 direction;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};
uniform Light light;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	int shininess;
};
uniform Material material;

uniform vec3 viewPos;

void main() {
	vec3 albedo_map = vec3(texture(material.diffuse, uv));
	vec3 specular_map = vec3(texture(material.specular, uv));

	vec3 ambient = albedo_map * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 /  (light.constant +
								light.linear * dist +
								light.quadratic * (dist * dist));

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	vec3 phong = (ambient + (diffuse * intensity) + (specular * intensity)) * attenuation;

	FragColor = vec4(phong, 1.0);
}