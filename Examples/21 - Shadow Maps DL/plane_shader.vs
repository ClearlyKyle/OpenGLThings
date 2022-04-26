#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec2 texCoord;

// Imports the camera matrix
uniform mat4 camMatrix;
// Imports the transformation matrices
uniform mat4 model;

void main()
{
    // Calculates current position
    crntPos = vec3(model * vec4(aPos, 1.0f));
    // Assigns the normal from the Vertex Data to "Normal"
    Normal = aNormal;
    // Assigns the texture coordinates from the Vertex Data to "texCoord"
    texCoord = aTex;
    // Outputs the positions/coordinates of all vertices
    gl_Position = camMatrix * vec4(crntPos, 1.0f);
    // gl_Position = vec4(aPos, 1.0);
}