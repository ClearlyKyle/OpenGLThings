#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

out vec2 TexCoords;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
}
vs_out;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 lightMatrix;

void main()
{
    vs_out.FragPos           = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal            = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords         = aTex;
    vs_out.FragPosLightSpace = lightMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position              = camMatrix * vec4(vs_out.FragPos, 1.0);
}