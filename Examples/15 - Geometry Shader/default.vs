#version 330 core

// Positions/Coordinates
layout(location = 0) in vec3 aPos;
// Normals (not necessarily normalized)
layout(location = 1) in vec3 aNormal;
// Texture Coordinates
layout(location = 2) in vec2 aTex;
// Colors
layout(location = 3) in vec3 aColor;

out DATA
{
    vec3 Normal;
    vec3 color;
    vec2 texCoord;
    mat4 projection;
}
data_out;

// Imports the camera matrix
uniform mat4 camMatrix;
// Imports the transformation matrices
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
    gl_Position         = model * translation * rotation * scale * vec4(aPos, 1.0f);
    data_out.Normal     = aNormal;
    data_out.color      = aColor;
    data_out.texCoord   = aTex;
    data_out.projection = camMatrix;
}