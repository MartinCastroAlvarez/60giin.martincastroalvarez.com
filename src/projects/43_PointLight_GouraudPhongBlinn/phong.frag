#version 410 core

in vec3 gouraudColor;
out vec4 FragColor;

void main() {
	FragColor = vec4(gouraudColor, 1.0);
}
