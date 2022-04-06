#ifndef __UBOEXAMPLE_H__
#define __UBOEXAMPLE_H__

#include "Engine.h"

struct UBOExample
{
    struct Shader shader_red;
    struct Shader shader_green;
    struct Shader shader_blue;
    struct Shader shader_yellow;
    struct VAO vao;
    struct UBO ubo;
    struct Camera cam;
};

void UBOExample_Init();
void UBOExample_Update();
void UBOExample_OnExit();

#endif // __UBOEXAMPLE_H__