#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aSides;

out vec3  vColor;
out float vSides;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor      = aColor;
    vSides      = aSides;
}