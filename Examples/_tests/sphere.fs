#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

// uniform vec3 camPos;

void main()
{
    vec3 lightPos = vec3(5.0, 0.0, 0.0);

    // diffuse
    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(lightPos - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);

    FragColor = diff * vec4(0.8f, 0.3f, 0.02f, 1.0f);
}