#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "Shaders.h"
#include "Camera.h"
#include "Window.h"

struct Skybox
{
    struct Shader shader;
    GLuint        VAO;
    GLuint        texture;
};

typedef struct Skybox Skybox_t;

Skybox_t Skybox_Create(struct Shader shader, const char *cube_map_face_images[6]);
Skybox_t Skybox_Create_Default();
void     Skybox_Draw(Skybox_t skybox, Camera_t cam);
void     Skybox_Free(Skybox_t skybox);

#endif // __SKYBOX_H__