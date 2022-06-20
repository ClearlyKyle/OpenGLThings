#ifndef __BALL_H__
#define __BALL_H__

#include "Engine.h"

typedef struct ball
{
    float   dx, dy;
    float   radius;
    vec3    pos;
    mat4    model;
    VAO_t   vao;
    GLsizei count;
} Ball_t;

Ball_t Ball_Init(const float x, const float y, const float radius);
void   Ball_Update(Ball_t *const ball);
void   Ball_Draw(const Shader_t shader, const Ball_t *const ball);
void   Ball_Destroy(Ball_t *const ball);

#endif // __BALL_H__