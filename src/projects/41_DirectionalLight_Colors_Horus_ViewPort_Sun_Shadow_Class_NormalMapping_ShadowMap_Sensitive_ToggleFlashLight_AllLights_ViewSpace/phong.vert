#version 410 core

// 2. STRUCTS
// (None)

// 3. FUNCS
// (None)

// 4. INPUTS
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTexCoord;
layout (location=2) in vec3 aNormal;
layout (location=3) in vec3 aTangent; // NORMAL MAPPING

uniform mat4 model;
uniform mat3 normalMat;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMatrix; // SHADOW MAP

out vec3 fragPos;
out vec4 fragPosLightSpace; // SHADOW MAP
out mat3 TBN; // NORMAL MAPPING
out vec2 uv;

// 5. MAIN
void main() {
	uv = aTexCoord;
	
    vec3 T = normalize(normalMat * aTangent); // NORMAL MAPPING
    vec3 N = normalize(normalMat * aNormal);  // NORMAL MAPPING
    T = normalize(T - dot(T, N) * N);         // NORMAL MAPPING
    vec3 B = cross(N, T);                     // NORMAL MAPPING
    TBN = mat3(T, B, N);                      // NORMAL MAPPING
	
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    fragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0); // SHADOW MAP

	gl_Position = proj * view * model * vec4(aPos, 1.0);
}