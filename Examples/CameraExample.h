#ifndef __CAMERAEXAMPLE_H__
#define __CAMERAEXAMPLE_H__

#include "util.h"

#include "glad/glad.h"

struct CameraExample
{
    struct Shader shader;
    struct VAO vao;
    struct Texture tex;
    struct Camera cam;

    GLsizei numer_of_indicies;
};

void CameraExample_Init();
void CameraExample_Update();
void CameraExample_OnExit();

#endif // __CAMERAEXAMPLE_H__