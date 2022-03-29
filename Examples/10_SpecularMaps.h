#ifndef __10_SPECULARMAPS_H__
#define __10_SPECULARMAPS_H__

#include "util.h"

#include "glad/glad.h"

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
#endif // __10_SPECULARMAPS_H__