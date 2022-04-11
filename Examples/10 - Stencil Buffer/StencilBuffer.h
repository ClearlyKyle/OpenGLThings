#ifndef __STENCILBUFFER_H__
#define __STENCILBUFFER_H__

#include "Engine.h"

struct StencilBuffer
{
    struct Camera cam;
    struct Mesh model;
    struct Mesh outline;
};

void StencilBuffer_Init();
void StencilBuffer_Update();
void StencilBuffer_OnExit();

#endif // __STENCILBUFFER_H__