#version 330 core

// FROM Attrib Arrays
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aColour;

// UNIFORMS
uniform mat4 camMatrix;

// Imports the transformation matrices
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

layout(std140) uniform Matrices
{
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    vec4  emission;
    float shininess;
};

// OUTPUTS to Fragment Shader
out vec4  fSpecular;
out vec4  fAmbient;
out vec4  fDiffuse;
out float fShininess;

out vec3 fCurrentPosition;
out vec3 fNormal;
out vec3 fColor;
out vec2 fTexCoords;

void main()
{
    // calculates current position
    fCurrentPosition = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
    // Assigns the normal from the Vertex Data to "Normal"
    fNormal = aNormal;
    // Assigns the colors from the Vertex Data to "color"
    fColor = aColour;
    // Assigns the texture coordinates from the Vertex Data to "texCoord"
    // fTexCoords = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
    fTexCoords = aTex;

    // Outputs the positions/coordinates of all vertices
    gl_Position = camMatrix * vec4(fCurrentPosition, 1.0);

    // OUTPUT to Fragment Shader
    fAmbient   = ambient;
    fDiffuse   = diffuse;
    fSpecular  = specular;
    fShininess = shininess;
}