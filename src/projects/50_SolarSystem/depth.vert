#version 410 core

layout (location=0) in vec3 aPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrixPhongBlinn;

void main() {
	gl_Position = lightSpaceMatrixPhongBlinn * model * vec4(aPos, 1.0);
}