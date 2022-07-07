#ifndef __BRICKS_H__
#define __BRICKS_H__

#include "Engine.h"

#define NUM_COLUMNS 8
#define NUM_ROW 6

typedef struct Bricks
{
    float   dx, dy;
    float   width, height;
    mat4    model[NUM_COLUMNS * NUM_ROW];
    vec3    pos[NUM_COLUMNS * NUM_ROW];
    bool    isHit[NUM_COLUMNS * NUM_ROW];
    VAO_t   vao;
    GLsizei vertex_count;

} Bricks_t;

Bricks_t Bricks_Init();
void     Bricks_Draw(const Shader_t shader, const Bricks_t *const bricks);
void     Bricks_Destroy(Bricks_t *const bricks);

#endif // __BRICKS_H__