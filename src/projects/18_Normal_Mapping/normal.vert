#version 460 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNormal;
layout (location=3) in vec3 aTangent;

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
};
uniform Light light;

uniform vec3 viewPos;

out vec2 uv;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

void main() {
	uv = aTexCoord;
	vec3 fragPos = vec3(model * vec4(aPos, 1.0));

	vec3 T = normalize(normalMat * aTangent);
	vec3 N = normalize(normalMat * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T,B,N));

	tangentLightPos = TBN * light.position;
	tangentViewPos = TBN * viewPos;
	tangentFragPos = TBN * fragPos;
   
	gl_Position = proj * view * model * vec4(aPos, 1.0);
}