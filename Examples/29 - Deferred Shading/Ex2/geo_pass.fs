#version 330

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;
layout(location = 3) out vec3 TexCoordOut;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

uniform sampler2D diffuse0;

void main()
{
    WorldPosOut = WorldPos0;
    DiffuseOut  = texture(diffuse0, TexCoord0).xyz;
    NormalOut   = normalize(Normal0);
    TexCoordOut = vec3(TexCoord0, 0.0);
}