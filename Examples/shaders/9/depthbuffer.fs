#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
//in vec3 Normal;
// Imports the color from the Vertex Shader
//in vec3 color;
// Imports the texture coordinates from the Vertex Shader
//in vec2 texCoord;



void main() {
  // outputs final color
  // float depth = logisticDepth(gl_FragCoord.z);
  // FragColor = direcLight() * (1.0f - depth) +
  //            vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);

  FragColor = vec4(0.8f, 0.6f, 0.4f, 1.0f);
}