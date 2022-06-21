#ifndef __BRICKS_H__
#define __BRICKS_H__

#include "Engine.h"

typedef struct Bricks
{
    float   dx, dy;
    float   width, height;
    mat4    model[36];
    vec3    pos[36];
    bool    isHit[36];
    VAO_t   vao;
    GLsizei count;

} Bricks_t;

Bricks_t Bricks_Init();
void     Bricks_Draw(const Shader_t shader, const Bricks_t *const bricks);
void     Bricks_Destroy(Bricks_t *const bricks);

#endif // __BRICKS_H__