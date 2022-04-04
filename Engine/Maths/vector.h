#ifndef __VEC4_H__
#define __VEC4_H__

#include <math.h>

typedef struct vec4
{
    float x, y, z, w;
} vec4_t;

typedef struct vec3
{
    float x, y, z;
} vec3_t;

typedef struct Mat4x4
{
    float m[4][4];
} mat4x4_t;

#define degreesToRadians(angleDegrees) ((angleDegrees)*M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians)*180.0 / M_PI)

#endif // __VEC4_H__