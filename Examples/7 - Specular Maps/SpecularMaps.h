#ifndef __SPECULARMAPS_H__
#define __SPECULARMAPS_H__

#include "Engine.h"

struct SpecularMaps
{
    struct Shader shader[2];
    struct VAO vao[2];
    struct Texture tex[2];
    struct Camera cam;

    GLsizei numer_of_indicies[2];
};

void SpecularMaps_Init();
void SpecularMaps_Update();
void SpecularMaps_OnExit();

#endif // __SPECULARMAPS_H__