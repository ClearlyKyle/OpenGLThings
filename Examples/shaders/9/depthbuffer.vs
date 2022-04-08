#version 330 core

// FROM Attrib Arrays
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aColour;

// UNIFORMS
uniform mat4 camMatrix;
uniform mat4 model;

layout(std140) uniform Matrices {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emission;
  float shininess;
};

// OUTPUTS to Fragment Shader
out vec4 fAmbient;
out vec4 fDiffuse;
out vec2 fTexCoords;

void main() {
  // calculates current position
  // crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
  gl_Position = camMatrix * model * vec4(aPos, 1.0);
  // crntPos = vec3(model * vec4(aPos, 1.0f));
  // Assigns the normal from the Vertex Data to "Normal"
  // Normal = aNormal;
  // Assigns the colors from the Vertex Data to "color"
  // color = aColor;
  // Assigns the texture coordinates from the Vertex Data to "texCoord"
  // texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

  // fColour = aColour;

  // OUTPUT to Fragment Shader
  fAmbient = ambient;
  fDiffuse = diffuse;
  fTexCoords = aTex;
}