#ifndef __SPOTLIGHT_H__
#define __SPOTLIGHT_H__

#include "Engine.h"

/* Shader struct

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

uniform Spotlight gSpotlight;
*/

struct Spotlight
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

    vec3  direction;
    float cutoff;
};

typedef struct Spotlight Spotlight_t;

#define SPOTPOINT_LIGHT_INIT                        \
    (Spotlight_t)                                   \
    {                                               \
        .colour               = {0.0f, 0.0f, 0.0f}, \
        .ambient_intensity    = 0.0f,               \
        .diffuse_intensity    = 0.0f,               \
        .position             = {0.0f, 0.0f, 0.0f}, \
        .attenuation.constant = 1.0f,               \
        .attenuation.linear   = 0.0f,               \
        .attenuation.exp      = 0.0f,               \
        .direction            = {0.0f, 0.0f, 0.0f}, \
        .cutoff               = 0.0f,               \
    }

inline void Spotlight_To_Shader(const Spotlight_t light, const Shader_t shader)
{
    Shader_Bind(shader);

    Shader_Uniform_Vec3(shader, "gSpotlight.Colour", light.colour);
    Shader_Uniform_Vec3(shader, "gSpotlight.Position", light.position);
    Shader_Uniform_Vec3(shader, "gSpotlight.Direction", light.direction);

    Shader_Uniform_Float(shader, "gSpotlight.AmbientIntensity", light.ambient_intensity);
    Shader_Uniform_Float(shader, "gSpotlight.DiffuseIntensity", light.diffuse_intensity);

    Shader_Uniform_Float(shader, "gSpotlight.Att_Constant", light.attenuation.constant);
    Shader_Uniform_Float(shader, "gSpotlight.Att_Linear", light.attenuation.linear);
    Shader_Uniform_Float(shader, "gSpotlight.Att_Exp", light.attenuation.exp);

    Shader_Uniform_Float(shader, "gSpotlight.Cutoff", light.cutoff);
}

#endif // __SPOTLIGHT_H__