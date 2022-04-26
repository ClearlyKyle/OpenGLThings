#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec2 texCoord;

// Gets the Texture Units from the main function
uniform sampler2D tex0;
// uniform vec3 camPos;

void main()
{
    FragColor = texture(tex0, texCoord);
    // FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}