#ifndef __CHALLANGES_H__
#define __CHALLANGES_H__

#include "Engine.h"

struct challanges
{
    struct VAO vao;
    struct Shader shader;

    GLsizei size;
};

void challanges_Init();
void challanges_Update();
void challanges_OnExit();

#endif // __CHALLANGES_H__