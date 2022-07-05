#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

#include "Engine.h"

/* Shader struct

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

uniform PointLight gPointLight;
*/

struct PointLight
{
    vec3  colour;
    float ambient_intensity;
    float diffuse_intensity;

    vec3 position;
    struct
    {
        float constant;
        float linear;
        float exp;
    } attenuation;
};

typedef struct PointLight PointLight_t;

#define POINT_LIGHT_INIT                            \
    (PointLight_t)                                  \
    {                                               \
        .colour               = {0.0f, 0.0f, 0.0f}, \
        .ambient_intensity    = 0.0f,               \
        .diffuse_intensity    = 0.0f,               \
        .position             = {0.0f, 0.0f, 0.0f}, \
        .attenuation.constant = 1.0f,               \
        .attenuation.linear   = 0.0f,               \
        .attenuation.exp      = 0.0f,               \
    }

void PointLight_To_Shader(const PointLight_t light, const Shader_t shader)
{
    Shader_Bind(shader);

    Shader_Uniform_Vec3(shader, "gPointLight.Colour", light.colour);
    Shader_Uniform_Vec3(shader, "gPointLight.Position", light.position);

    Shader_Uniform_Float(shader, "gPointLight.AmbientIntensity", light.ambient_intensity);
    Shader_Uniform_Float(shader, "gPointLight.DiffuseIntensity", light.diffuse_intensity);

    Shader_Uniform_Float(shader, "gPointLight.Att_Constant", light.attenuation.constant);
    Shader_Uniform_Float(shader, "gPointLight.Att_Linear", light.attenuation.linear);
    Shader_Uniform_Float(shader, "gPointLight.Att_Exp", light.attenuation.exp);
}

#endif // __POINT_LIGHT_H__