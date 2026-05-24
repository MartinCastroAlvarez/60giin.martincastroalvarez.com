#version 410 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D depth_map;

void main() {
	float depthValue = texture(depth_map, uv).r;
	FragColor = vec4(vec3(depthValue), 1.0);
}