#ifndef __MODELLOADINGEXAMPLE_H__
#define __MODELLOADINGEXAMPLE_H__

#include "util.h"

#include "glad/glad.h"

struct ModelLoading
{
    struct Shader shader;
    struct VAO vao;

    struct Model model;
    struct Camera camera;
};

void ModelLoading_Init();
void ModelLoading_Update();
void ModelLoading_OnExit();

#endif // __MODELLOADINGEXAMPLE_H__