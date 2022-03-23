#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Shaders.h"

enum Shader_Type
{
    SHADER_NONE = 0,
    SHDAER_BASIC_TRIANGLE,
    SHADER_TYPE_COUNT
};

struct Renderer
{
    struct Shader shaders[SHADER_TYPE_COUNT + 1];
    enum ShaderType current_shader;
    struct Shader shader;
};

#endif // __RENDERER_H__