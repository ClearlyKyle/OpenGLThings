#include "vector.h"

vec3_t vec3_add_vec3(const vec3_t v1, const vec3_t v2)
{
    return (vec3_t){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3_t vec3_sub_vec3(const vec3_t v1, const vec3_t v2)
{
    return (vec3_t){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

vec3_t Vector_Mul(const vec3_t v1, float k)
{
    return (vec3_t){v1.x * k, v1.y * k, v1.z * k};
}

vec3_t Vector_Div(const vec3_t v1, float k)
{
    return (vec3_t){v1.x / k, v1.y / k, v1.z / k};
}

float Vector_Dot_Product(const vec3_t v1, const vec3_t v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector_Length(const vec3_t v)
{
    return sqrtf(Vector_Dot_Product(v, v));
}

vec3_t Vector_Normalise(const vec3_t v)
{
    const float l = Vector_Length(v);
    return (vec3_t){v.x / l, v.y / l, v.z / l};
}
