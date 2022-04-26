#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 spaceMatrix;

void main()
{
    gl_Position = spaceMatrix * model * vec4(position, 1.0f);
}