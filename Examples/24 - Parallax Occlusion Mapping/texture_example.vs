#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTex;

out vec3 color;    // Outputs the color for the Fragment Shader
out vec2 texCoord; // Outputs the texture coordinates to the fragment shader

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 model;
uniform mat4 camMatrix;

void main()
{
    gl_Position = camMatrix * model * translation * rotation * scale * vec4(aPos, 1.0);

    color = aColor;

    texCoord = aTex;
}