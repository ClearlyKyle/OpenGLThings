#version 330 core

out vec4 FragColor;

// Imports from the Vertex Shader
in vec3 Normal;
in vec3 crntPos;
in vec2 texCoord;

// Gets the position of the camera from the main function
uniform vec3 camPos;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;

vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

vec4 direcLight() {
  // ambient lighting
  float ambient = 0.20f;

  // diffuse lighting
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(camPos - crntPos);
  float diffuse = max(dot(normal, lightDir), 0.0f);

  return (texture(tex0, texCoord) * (diffuse + ambient)) * lightColor;
}

void main() {
  FragColor = direcLight();
  //  FragColor = vec4(lightColor, 1.0f);
}