#ifndef __TEXTUREEXAMPLE_H__
#define __TEXTUREEXAMPLE_H__

#include "Engine.h"

struct TextureExample
{
    struct Shader shader;
    struct VAO vao;
    struct Texture tex;
};

void TextureExample_Init();
void TextureExample_Update();

#endif // __TEXTUREEXAMPLE_H__