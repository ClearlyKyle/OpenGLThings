#version 330 core

// Positions/Coordinates
layout(location = 0) in vec3 aPos;
// Normals (not necessarily normalized)
layout(location = 1) in vec3 aNormal;
// Texture Coordinates
layout(location = 2) in vec2 aTex;
// Colors
layout(location = 3) in vec3 aColor;
// Instancing Transformations
layout(location = 4) in mat4 instanceMatrix;
// 5, 6, 7 - taken with mat4 instanceMatrix

// Outputs the current position for the Fragment Shader
out vec3 crntPos;
// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;

// Imports the camera matrix
uniform mat4 camMatrix;

void main()
{
    // calculates current position
    crntPos = vec3(instanceMatrix * vec4(aPos, 1.0f));
    // Assigns the normal from the Vertex Data to "Normal"
    Normal = aNormal;
    // Assigns the colors from the Vertex Data to "color"
    color = aColor;
    // Assigns the texture coordinates from the Vertex Data to "texCoord"
    texCoord = aTex;

    // Outputs the positions/coordinates of all vertices
    gl_Position = camMatrix * vec4(crntPos, 1.0);
}