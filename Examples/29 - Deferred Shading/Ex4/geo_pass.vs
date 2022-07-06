#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

uniform mat4 camMatrix;
uniform mat4 model;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main()
{
    gl_Position = camMatrix * model * vec4(Position, 1.0);
    TexCoord0   = TexCoord;
    Normal0     = (model * vec4(Normal, 0.0)).xyz;
    WorldPos0   = (model * vec4(Position, 1.0)).xyz;
}