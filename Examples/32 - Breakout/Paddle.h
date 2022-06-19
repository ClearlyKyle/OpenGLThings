#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "Engine.h"

typedef struct Paddle
{
    float   dx, dy;
    vec3    pos;
    mat4    model;
    VAO_t   vao;
    GLsizei count;
} Paddle_t;

Paddle_t Paddle_Init(const float y);
void     Paddle_Update(Paddle_t *const paddle);
void     Paddle_Draw(const Shader_t shader, const Paddle_t *const paddle);
void     Paddle_Destroy(Paddle_t *const paddle);

#endif // __PADDLE_H__
