#version 410 core

in vec3 fragPos;
in vec4 fragPosLightSpace;
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

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	int shininess;
};
uniform Material material;

uniform vec3 viewPos;

uniform sampler2D depthMap;

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
//	float closestDepth = texture(depthMap, projCoords.xy).r;

	float currentDepth = projCoords.z;
//	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0) {
		shadow = 0.0;
	}

	return shadow;
}

void main() {
	vec3 albedo_map = vec3(texture(material.diffuse, uv));
	vec3 specular_map = vec3(texture(material.specular, uv));

	vec3 ambient = albedo_map * light.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = spec * specular_map * light.specular;

	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	float shadow = ShadowCalculation(fragPosLightSpace, bias);

	vec3 phong = ambient + ((1.0 - shadow) * (diffuse + specular));
	FragColor = vec4(phong, 1.0);
}