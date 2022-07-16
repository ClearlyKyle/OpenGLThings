#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aColor;

out vec3 crntPos;  // Outputs the current position for the Fragment Shader
out vec3 Normal;   // Outputs the normal for the Fragment Shader
out vec3 color;    // Outputs the color for the Fragment Shader
out vec2 texCoord; // Outputs the texture coordinates to the Fragment Shader

// Imports the camera matrix
uniform mat4 camMatrix;

// Imports the transformation matrices
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
    // calculates current position
    crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));

    Normal   = aNormal;
    color    = aColor;
    texCoord = aTex;

    // Outputs the positions/coordinates of all vertices
    gl_Position = camMatrix * vec4(crntPos, 1.0);
}