#version 330 core

out vec4 FragColor; // Outputs colors in RGBA

in vec3 color;    // Inputs the color from the Vertex Shader
in vec2 texCoord; // Inputs the texture coordinates from the Vertex Shader

uniform sampler2D tex0; // Gets the Texture Unit from the main function

void main()
{
    FragColor = texture(tex0, texCoord);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}