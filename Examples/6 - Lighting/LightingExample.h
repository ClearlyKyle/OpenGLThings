#ifndef __LIGHTINGEXAMPLE_H__
#define __LIGHTINGEXAMPLE_H__

#include "Engine.h"

struct LightingExample
{
    struct Shader shader[2];
    struct VAO vao[2];
    struct Texture tex;
    struct Camera cam;

    GLsizei numer_of_indicies;
};

void LightingExample_Init();
void LightingExample_Update();
void LightingExample_OnExit();

#endif // __LIGHTINGEXAMPLE_H__