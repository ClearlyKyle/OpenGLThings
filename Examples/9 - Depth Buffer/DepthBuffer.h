#ifndef __DEPTHBUFFER_H__
#define __DEPTHBUFFER_H__

#include "Engine.h"

struct DepthBuffer
{
    struct Shader shader;
    struct VAO vao;
    struct Camera cam;
    struct Mesh model;
};

void DepthBuffer_Init();
void DepthBuffer_Update();
void DepthBuffer_OnExit();

#endif // __DEPTHBUFFER_H__