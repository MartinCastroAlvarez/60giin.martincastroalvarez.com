#version 410 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D screen_text;

void main() {
	FragColor = texture(screen_text, uv);
}