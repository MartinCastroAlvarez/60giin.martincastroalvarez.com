#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;

out vec3 fragPos;
out vec3 normal;

out vec4 fragPosLightSpace;
uniform mat4 lightSpaceMatrixPhongBlinn;
uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

void main() {
    vec3 fragPosWorld = vec3(model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrixPhongBlinn * vec4(fragPosWorld, 1.0);
    fragPos = vec3(view * vec4(fragPosWorld, 1.0));
    normal = normalMat * aNormal;
    gl_Position = proj * vec4(fragPos, 1.0);
}
