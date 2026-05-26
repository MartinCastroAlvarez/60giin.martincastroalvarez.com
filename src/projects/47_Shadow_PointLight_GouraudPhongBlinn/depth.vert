#version 410 core

layout (location=0) in vec3 aPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrixGouraud;

void main() {
	gl_Position = lightSpaceMatrixGouraud * model * vec4(aPos, 1.0);
}