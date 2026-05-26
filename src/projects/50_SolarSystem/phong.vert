#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec3 normal;
out vec2 uv;
out vec3 gouraudColor;

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrixPhongBlinn;

uniform Light lightGouraud;
uniform vec3 objectColor;

void main() {
    vec3 fragPosWorld = vec3(model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrixPhongBlinn * vec4(fragPosWorld, 1.0);
    
    fragPos = vec3(view * vec4(fragPosWorld, 1.0));
    normal = normalMat * aNormal;
    uv = aUv;
    gl_Position = proj * vec4(fragPos, 1.0);

    // Gouraud calculation
    vec3 normalView = normalize(normalMat * aNormal);
    vec3 lightDirView = normalize(lightGouraud.position - fragPos);
    float diff = max(dot(normalView, lightDirView), 0.0);
    vec3 viewDirView = normalize(-fragPos);
    vec3 reflectDirView = reflect(-lightDirView, normalView);
    float spec = pow(max(dot(viewDirView, reflectDirView), 0.0), 32);
    
    float distance = length(lightGouraud.position - fragPos);
    float attenuation = 1.0 / (lightGouraud.constant + lightGouraud.linear * distance + lightGouraud.quadratic * (distance * distance));
    
    vec3 ambient = lightGouraud.ambient * objectColor;
    vec3 diffuse = lightGouraud.diffuse * diff * objectColor;
    vec3 specular = lightGouraud.specular * spec;
    
    gouraudColor = (ambient + diffuse + specular) * attenuation;
}
