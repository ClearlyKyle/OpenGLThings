#ifndef __BASICTRAINGLE_H__
#define __BASICTRAINGLE_H__

#include "util.h"

#include "glad/glad.h"

struct BasicTriangle
{
    struct Shader shader;
    struct VAO vao;
};

void BasicTriangle_Init();
void BasicTriangle_Update();

#endif // __BASICTRAINGLE_H__