#version 330 core

out vec4 FragColor;

// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the current position from the Vertex Shader
in vec3 crntPos;

// Gets the position of the camera from the main function
uniform vec3 camPos;

vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

vec3 direcLight() {
  // ambient lighting
  float ambient = 0.20f;

  // diffuse lighting
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(camPos - crntPos);
  float diffuse = max(dot(normal, lightDir), 0.0f);

  return (diffuse + ambient) * lightColor;
}

void main() {
   FragColor = vec4(direcLight(), 1.0f);
//  FragColor = vec4(lightColor, 1.0f);
}