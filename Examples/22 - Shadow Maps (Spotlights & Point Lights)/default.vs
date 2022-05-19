#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aColor;

out vec3 crntPos;      // Outputs the current position for the Fragment Shader
out vec3 Normal;       // Outputs the normal for the Fragment Shader
out vec3 color;        // Outputs the color for the Fragment Shader
out vec2 texCoord;     // Outputs the texture coordinates to the Fragment Shader
out vec4 fragPosLight; // Outputs the fragment position of the light

// Imports the camera matrix
uniform mat4 camMatrix;
// Imports the transformation matrices
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
// Imports the light matrix
uniform mat4 lightProjection;

void main()
{
    // Calculates current position
    crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
    // Assigns the normal from the Vertex Data to "Normal"
    Normal = aNormal;
    // Assigns the colors from the Vertex Data to "color"
    color = aColor;
    // Assigns the texture coordinates from the Vertex Data to "texCoord"
    texCoord = aTex;
    // Calculates the position of the light fragment for the fragment shader
    fragPosLight = lightProjection * vec4(crntPos, 1.0f);

    // Outputs the positions/coordinates of all vertices
    gl_Position = camMatrix * vec4(crntPos, 1.0);
}