#ifndef __SHADERCOLOUR_H__
#define __SHADERCOLOUR_H__

#include "util.h"

struct ShaderColour
{
    struct Shader shader;
    struct VAO vao;
};

void ShaderColour_Init();
void ShaderColour_Update();

#endif // __SHADERCOLOUR_H__