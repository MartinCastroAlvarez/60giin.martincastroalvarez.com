#version 410 core

// 2. STRUCTS
struct DirLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

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


// 3. FUNCS
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, sampler2D depthMap) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float currentDepth = projCoords.z;

	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
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

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo_map, vec3 specular_map, float shadow, float shadowIntensity, int shininess) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	// BLINN-PHONG REFLECTION MODEL
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = spec * specular_map * light.specular;

	return ambient + (1.0 - (shadow * shadowIntensity)) * (diffuse + specular); // SHADOW MAP
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo_map, vec3 specular_map, int shininess) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 /  (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo_map * light.diffuse;

	// BLINN-PHONG REFLECTION MODEL
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	vec3 specular = spec * specular_map * light.specular;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedo_map, vec3 specular_map, int shininess) {
	vec3 ambient = albedo_map * light.ambient;

	vec3 lightDir = light.position - fragPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	float attenuation = 1.0 /  (light.constant + light.linear * dist + light.quadratic * (dist * dist));

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

// 4. INPUTS
in vec3 fragPos;
in vec4 fragPosLightSpace; // SHADOW MAP
in mat3 TBN; // NORMAL MAPPING
in vec2 uv;

uniform DirLight dirLight;
uniform PointLight pointLight0;
uniform PointLight pointLight1;
uniform PointLight pointLight2; // SUN
uniform SpotLight spotLight0;
uniform SpotLight spotLight1;
uniform SpotLight spotLight2; // Flashlight
uniform float normalIntensity; // NORMAL MAPPING
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform int shininess;
uniform sampler2D depthMap;
uniform float shadowIntensity;

out vec4 FragColor;

// 5. MAIN
void main() {
	vec3 albedo_map = vec3(texture(texture_diffuse1, uv));
	vec3 specular_map = vec3(texture(texture_specular1, uv));
	vec3 normal_map = vec3(texture(texture_normal1, uv)); // NORMAL MAPPING

	vec3 mapped_normal = normal_map * 2.0 - 1.0; // NORMAL MAPPING
	mapped_normal.xy *= normalIntensity; // NORMAL MAPPING
	vec3 norm = normalize(TBN * normalize(mapped_normal)); // NORMAL MAPPING

	vec3 viewDir = normalize(-fragPos);

	float shadow = ShadowCalculation(fragPosLightSpace, norm, normalize(-dirLight.direction), depthMap); // SHADOW MAP
	vec3 phong = calcDirectionalLight(dirLight, norm, viewDir, albedo_map, specular_map, shadow, shadowIntensity, shininess);

	phong += calcPointLight(pointLight0, norm, viewDir, fragPos, albedo_map, specular_map, shininess);
	phong += calcPointLight(pointLight1, norm, viewDir, fragPos, albedo_map, specular_map, shininess);
	// phong += calcPointLight(pointLight2, norm, viewDir, fragPos, albedo_map, specular_map, shininess); // SUN (Disabled to not overwrite shadows)

	phong += calcSpotLight(spotLight0, norm, viewDir, fragPos, albedo_map, specular_map, shininess);
	phong += calcSpotLight(spotLight1, norm, viewDir, fragPos, albedo_map, specular_map, shininess);
	phong += calcSpotLight(spotLight2, norm, viewDir, fragPos, albedo_map, specular_map, shininess);

	FragColor = vec4(phong, 1.0);
}