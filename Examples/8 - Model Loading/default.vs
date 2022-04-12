#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

// Outputs for the Fragment Shader
out vec3 Normal;
out vec2 texCoord;
out vec3 crntPos;

// Imports the camera matrix from the main function
uniform vec3 camPos;
uniform mat4 model;
uniform mat4 camMatrix;

void main() {
  // calculates current position
  crntPos = vec3(model * vec4(aPos, 1.0f));
  // Outputs the positions/coordinates of all vertices
  gl_Position = camMatrix * vec4(crntPos, 1.0);

  texCoord = aTex;
  Normal = aNormal;

}