#version 330

struct DirectionalLight
{
    vec3  Colour;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3  Direction;
};

struct Spotlight
{
    vec3  Colour;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3  Position;
    vec3  Direction;
    float Cutoff;
    float Att_Constant;
    float Att_Linear;
    float Att_Exp;
};

struct PointLight
{
    vec3  Colour;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3  Position;
    float Att_Constant;
    float Att_Linear;
    float Att_Exp;
};

uniform DirectionalLight gDirectionalLight;
uniform Spotlight        gSpotlight;
uniform PointLight       gPointLight;

// These come from the gBuffer, these are not actual textures
// look at Diferred Ex2 to see where they come from
uniform sampler2D gPositionMap;
uniform sampler2D gColorMap;
uniform sampler2D gNormalMap;

uniform vec3  gCameraPosition;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;
uniform vec2  gScreenSize;

vec4 CalcLightInternal(vec3  LightColour,
                       float AmbientIntensity,
                       float DiffuseIntensity,
                       vec3  LightDirection,
                       vec3  WorldPos,
                       vec3  Normal)
{
    vec4  AmbientColor  = vec4(LightColour * AmbientIntensity, 1.0);
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0)
    {
        DiffuseColor = vec4(LightColour * DiffuseIntensity * DiffuseFactor, 1.0);

        vec3  VertexToEye    = normalize(gCameraPosition - WorldPos);
        vec3  LightReflect   = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if (SpecularFactor > 0.0)
        {
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor  = vec4(LightColour * gMatSpecularIntensity * SpecularFactor, 1.0);
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 CalcDirectionalLight(vec3 WorldPos, vec3 Normal)
{
    return CalcLightInternal(gDirectionalLight.Colour,
                             gDirectionalLight.AmbientIntensity,
                             gDirectionalLight.DiffuseIntensity,
                             gDirectionalLight.Direction,
                             WorldPos,
                             Normal);
}

vec4 CalcPointLight(vec3 WorldPos, vec3 Normal)
{
    vec3  LightDirection = WorldPos - gPointLight.Position;
    float Distance       = length(LightDirection);
    LightDirection       = normalize(LightDirection);

    vec4 Color = CalcLightInternal(gPointLight.Colour,
                                   gPointLight.AmbientIntensity,
                                   gPointLight.DiffuseIntensity,
                                   LightDirection,
                                   WorldPos,
                                   Normal);

    float AttenuationFactor = gPointLight.Att_Constant +
                              gPointLight.Att_Linear * Distance +
                              gPointLight.Att_Exp * Distance * Distance;

    AttenuationFactor = max(1.0, AttenuationFactor);

    return Color / AttenuationFactor;
}

vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

out vec4 FragColor;

void main()
{
    vec2 TexCoord = CalcTexCoord();
    vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
    vec3 Color    = texture(gColorMap, TexCoord).xyz;
    vec3 Normal   = texture(gNormalMap, TexCoord).xyz;
    Normal        = normalize(Normal);

    FragColor = vec4(Color, 1.0) * CalcPointLight(WorldPos, Normal);
}
