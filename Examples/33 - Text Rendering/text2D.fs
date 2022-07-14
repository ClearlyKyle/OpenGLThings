#version 330 core

in vec2 TexCoords;

out vec4 colour;

uniform sampler2D textTexture;
uniform vec3      textColour;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textTexture, TexCoords).a);
    colour       = vec4(textColour, 1.0) * sampled;

    // colour = vec4(0.0, 1.0, 0.0, 1.0);
}