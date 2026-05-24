#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNormal;

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out vec3 normal;
out vec2 uv;

void main() {
	uv = aTexCoord;
	normal = normalMat * aNormal;
	
    fragPos = vec3(view * model * vec4(aPos, 1.0));

	gl_Position = proj * view * model * vec4(aPos, 1.0);
}