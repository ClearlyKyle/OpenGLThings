#ifndef __BASICTRAINGLE_H__
#define __BASICTRAINGLE_H__

#include "Engine.h"

struct BasicTriangle
{
    struct Shader shader;
    struct VAO vao;
};

void BasicTriangle_Init();
void BasicTriangle_Update();
void BasicTriangle_OnExit();

#endif // __BASICTRAINGLE_H__