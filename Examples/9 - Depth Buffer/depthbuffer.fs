#version 330 core
out vec4 FragColor;

// INPUTS from Vertex Shader
in vec3 fCurrentPosition;
in vec3 fNormal;
in vec3 fColor;
in vec2 fTexCoords;

in vec4  fSpecular;
in vec4  fAmbient;
in vec4  fDiffuse;
in float fShininess;

// TEXTURE DATA
uniform sampler2D diffuseTex;

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;

// Gets the color of the light from the main function
uniform vec4 lightColor;

// Gets the position of the light from the main function
uniform vec3 lightPos;

// Gets the position of the camera from the main function
uniform vec3 camPos;
uniform vec3 camFront;

vec4 pointLight()
{
    // used in two variables so I calculate it here to not have to do it twice
    vec3 lightVec = lightPos - fCurrentPosition;

    // intensity of light with respect to distance
    float dist  = length(lightVec);
    float a     = 3.0;
    float b     = 0.7;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

    // ambient lighting
    float ambient = 0.20f;

    // diffuse lighting
    vec3  fNormal        = normalize(fNormal);
    vec3  lightDirection = normalize(lightVec);
    float diffuse        = max(dot(fNormal, lightDirection), 0.0f);

    // specular lighting
    float specularLight       = 0.50f;
    vec3  viewDirection       = normalize(camPos - fCurrentPosition);
    vec3  reflectionDirection = reflect(-lightDirection, fNormal);
    float specAmount =
        pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return (texture(diffuse0, fTexCoords) * (diffuse * inten + ambient) +
            texture(specular0, fTexCoords).r * specular * inten) *
           lightColor;
}

vec4 direcLight()
{
    // ambient lighting
    float ambient = 0.6f;

    // diffuse lighting
    vec3  fNormal        = normalize(fNormal);
    vec3  lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse        = max(dot(fNormal, lightDirection), 0.0f);

    // specular lighting
    float specularLight       = 0.50f;
    vec3  viewDirection       = normalize(camPos - fCurrentPosition);
    vec3  reflectionDirection = reflect(-lightDirection, fNormal);
    float specAmount =
        pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    return (texture(diffuse0, fTexCoords) * (diffuse + ambient) +
            texture(specular0, fTexCoords).r * specular) *
           lightColor;
}

vec4 direct_lighting_with_material_data()
{
    // ambient lighting
    vec4 ambient_colour = 0.25 * fAmbient;

    // diffuse lighting
    vec3  fNormal        = normalize(fNormal);
    vec3  lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse        = max(dot(fNormal, lightDirection), 0.0f);
    vec4  diffuse_colour = lightColor * diffuse * fDiffuse;

    // specular lighting
    float specularLight       = 0.50f;
    vec3  viewDirection       = normalize(camPos - fCurrentPosition);
    vec3  reflectionDirection = reflect(-lightDirection, fNormal);
    float specAmount          = pow(max(dot(viewDirection, reflectionDirection), 0.0f), fShininess);
    float specular            = texture(specular0, fTexCoords).r * specAmount * specularLight;
    vec4  specular_colour     = lightColor * specular * fSpecular;

    // return (texture(diffuse0, fTexCoords) * (diffuse + ambient) +
    //         texture(specular0, fTexCoords).r * specular) *
    //        lightColor;
    return texture(diffuse0, fTexCoords) * (ambient_colour + diffuse_colour + specular_colour);
}

vec4 spotlight_lighting_with_material_data()
{
    // controls how big the area that is lit up is
    float outerCone = 0.952f; // cos(theta) : cos(radians(12.5f))
    float innerCone = 0.978f; // cos(theta) : cos(radians(17.5f))

    // ambient lighting
    float ambient        = 0.08;
    vec4  ambient_colour = texture(diffuse0, fTexCoords) * ambient * fAmbient;

    // diffuse lighting
    vec3  fNormal        = normalize(fNormal);
    vec3  lightDirection = normalize(camPos - fCurrentPosition);
    float diffuse        = max(dot(fNormal, lightDirection), 0.0);
    vec4  diffuse_colour = lightColor * diffuse * texture(diffuse0, fTexCoords) * fDiffuse;

    // specular lighting
    float specularLight       = 0.50f;
    vec3  viewDirection       = normalize(camPos - fCurrentPosition);
    vec3  reflectionDirection = reflect(-lightDirection, fNormal);
    float specular            = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    vec4  specular_colour     = lightColor * specular * fSpecular * specularLight;

    // calculates the intensity of the crntPos based on its angle to the center of the light cone
    float theta     = dot(lightDirection, normalize(-camFront));
    float epsilon   = (innerCone - outerCone);
    float intensity = clamp((theta - outerCone) / epsilon, 0.0, 1.0);
    diffuse_colour *= intensity;
    specular_colour *= intensity;

    // attenuation
    float distance    = length(camPos - fCurrentPosition);
    float attenuation = 1.0 / (1.0 + (0.09 * distance) + (0.032 * (distance * distance)));
    ambient_colour *= attenuation;
    diffuse_colour *= attenuation;
    specular_colour *= attenuation;

    // return (texture(diffuse0, fTexCoords) * (diffuse * intensity + ambient) +
    //         texture(specular0, fTexCoords).r * specular * intensity) *
    //        lightColor;
    return ambient_colour + diffuse_colour + specular_colour;
}

float near = 0.1f;
float far  = 100.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness, float offset)
{
    float zVal = linearizeDepth(depth);
    return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
    // FragColor = direct_lighting_with_material_data();
    vec4 res  = vec4(spotlight_lighting_with_material_data().rgb, 1.0);
    FragColor = res;
}