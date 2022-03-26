#ifndef __BASIC3D_H__
#define __BASIC3D_H__

#include "util.h"

#include "glad/glad.h"

struct Basic3D
{
    struct Shader shader;
    struct VAO vao;
    struct Texture tex;
    GLsizei numer_of_indicies;
};

void Basic3D_Init();
void Basic3D_Update();
void Basic3D_OnExit();

#endif // __BASIC3D_H__