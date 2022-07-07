#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 orthograph;
uniform vec3 pos;

void main()
{
    gl_Position = orthograph * model * vec4(aPos.xy, 0.0, 1.0);
}