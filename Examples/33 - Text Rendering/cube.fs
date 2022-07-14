#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

vec3 lightPos = vec3(3.0, 5.0, 2.0);

vec3 calc_lights()
{
    // ambient
    float ambientStrength = 0.1;
    vec3  ambient         = ambientStrength * vec3(1.0, 1.0, 1.0);

    // diffuse
    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(lightPos - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * vec3(0.5, 0.5, 0.5);

    vec3 result = (ambient + diffuse) * vec3(0.6, 0.0, 0.0);

    return result;
}

void main()
{
    FragColor = vec4(calc_lights(), 1.0f);
}