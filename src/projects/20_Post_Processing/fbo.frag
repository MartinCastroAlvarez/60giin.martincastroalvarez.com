#version 460 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D screen_text;

const float offset = 1.0 / 200.0;

void main() {
	//FragColor = texture(screen_text, uv);

	//FragColor = vec4(vec3(1.0 - texture(screen_text, uv)), 1.0)

//	vec4 color = texture(screen_text, uv);
//    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
//    FragColor = vec4(average, average, average, 1.0);

    vec2 offsets[9] = vec2[](
            vec2(-offset, offset),  //top left
            vec2(0.0, offset),      //top center
            vec2(offset, offset),   //top right
            vec2(-offset, 0.0),     //center left
            vec2(0.0, 0.0),         //center
            vec2(offset, 0.0),      //center right
            vec2(-offset, -offset), //bottom left
            vec2(0.0, -offset),     //bottom center
            vec2(offset, -offset)   // bottom right
    );

//    float kernel[9] = float[] (
//        -1, -1, -1,
//        -1, 9, -1,
//        -1, -1, -1
//    );

//float kernel[9] = float[] (
//    1.0/16, 2.0/16, 1.0/16,
//    2.0/16, 4.0/16, 2.0/16,
//    1.0/16, 2.0/16, 1.0/16
//   };
    
    float kernel[9] = float[] (
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
);

  vec3 sampleTex[9];
  for (int i = 0; i < 9; ++i) {
    sampleTex[i] = vec3(texture(screen_text, uv.st + offsets[i]));
  }

  vec3 color = vec3(0.0);

  for (int i = 0; i < 9; ++i) {
    color += sampleTex[i] * kernel[i];
  }

  FragColor = vec4(color, 1.0);
}