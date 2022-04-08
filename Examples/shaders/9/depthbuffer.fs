#version 330 core
out vec4 FragColor;

// INPUTS from Vertex Shader
in vec4 fAmbient;
in vec4 fDiffuse;
in vec2 fTexCoords;

// TEXTURE DATA
uniform sampler2D diffuseTex;

void main() {
  //  FragColor = vec4(1.0, 1.0, 0.0, 1.0);
  // FragColor = fDiffuse;
  FragColor = texture(diffuseTex, fTexCoords);
}