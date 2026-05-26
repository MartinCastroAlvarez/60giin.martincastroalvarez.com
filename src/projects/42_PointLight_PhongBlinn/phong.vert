#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUv;
layout (location=2) in vec3 aNormal;

out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

void main() {
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    normal = normalMat * aNormal;
    gl_Position = proj * vec4(fragPos, 1.0);
}
