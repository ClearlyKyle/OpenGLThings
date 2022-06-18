#version 330 core

layout(location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 orthograph;
uniform vec3 pos;

void main()
{
    // vec3 update_pos = vec3(aPos, 0.0) + pos;
    // vec4 pixel_pos  = mvp * vec4(vec3(aPos, 0.0), 0.0);
    //  gl_Position     = orthograph * pixel_pos;
    gl_Position = orthograph * model * vec4(aPos.xy, 0.0, 1.0);
}