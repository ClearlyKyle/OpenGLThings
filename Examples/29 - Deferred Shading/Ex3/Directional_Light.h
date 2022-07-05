#ifndef __DIRECTIONAL_LIGHT_H__
#define __DIRECTIONAL_LIGHT_H__

#include "Engine.h"

/* Shader struct

struct DirectionalLight
{
    vec3  Colour;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3  Direction;
};

uniform DirectionalLight gDirectionalLight;
*/

struct DirectionalLight
{
    vec3  colour;
    float ambient_intensity;
    float diffuse_intensity;

    vec3 direction;
};

typedef struct DirectionalLight DirectionalLight_t;

#define DIRECTIONAL_LIGHT_INIT                   \
    (DirectionalLight_t)                         \
    {                                            \
        .colour            = {0.0f, 0.0f, 0.0f}, \
        .ambient_intensity = 0.0f,               \
        .diffuse_intensity = 0.0f,               \
        .direction         = {0.0f, 0.0f, 0.0f}, \
    }

void DirectionalLight_To_Shader(const DirectionalLight_t light, const Shader_t shader)
{
    Shader_Bind(shader);

    Shader_Uniform_Vec3(shader, "gDirectionalLight.Colour", light.colour);
    Shader_Uniform_Vec3(shader, "gDirectionalLight.Direction", light.direction);

    Shader_Uniform_Float(shader, "gDirectionalLight.AmbientIntensity", light.ambient_intensity);
    Shader_Uniform_Float(shader, "gDirectionalLight.DiffuseIntensity", light.diffuse_intensity);
}

#endif // __DIRECTIONAL_LIGHT_H__