#version 460 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texture_1;
uniform sampler2D texture_2;

void main() {
	FragColor = mix(texture(texture_1, texCoord), texture(texture_2, texCoord), 0.1);
}