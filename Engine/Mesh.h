#ifndef __MESH_H__
#define __MESH_H__

#include "util.h"

#include "Camera.h"
#include "VAO.h"
#include "EBO.h"

struct MMesh
{
    VAO_t   vao;
    GLsizei num_indicies;

    Texture_t *textures;
    GLsizei    num_textures;

    mat4 translation;
    mat4 rotation;
    mat4 scale;
    mat4 matrix;
};

typedef struct MMesh MMesh_t;

MMesh_t Mesh_Construct(const GLfloat *verticies,
                       const GLuint  *indicies,
                       const GLsizei  num_indicies,
                       const GLsizei  num_textures,
                       Texture_t     *textures);

void MMesh_Draw(const MMesh_t mesh, const Shader_t shader, const Camera_t camera);

#endif // __MESH_H__