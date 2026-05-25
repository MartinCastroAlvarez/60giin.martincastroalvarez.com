#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNormal;
layout (location=3) in vec3 aTangent; // NORMAL MAPPING

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out mat3 TBN; // NORMAL MAPPING
out vec2 uv;

void main() {
	uv = aTexCoord;
	
    vec3 T = normalize(normalMat * aTangent); // NORMAL MAPPING
    vec3 N = normalize(normalMat * aNormal);  // NORMAL MAPPING
    T = normalize(T - dot(T, N) * N);         // NORMAL MAPPING
    vec3 B = cross(N, T);                     // NORMAL MAPPING
    TBN = mat3(T, B, N);                      // NORMAL MAPPING
	
    fragPos = vec3(view * model * vec4(aPos, 1.0));

	gl_Position = proj * view * model * vec4(aPos, 1.0);
}