#ifndef __MODELLOADER_H__
#define __MODELLOADER_H__

#include "util.h"

#include "VAO.h"
#include "EBO.h"
#include "Shaders.h"

struct Model
{
    struct VAO vao;
    struct Shader shader;
    struct Texture tex;

    GLuint num_indicies;
    GLuint num_of_verticies;
};

struct Model Model_Import(const char *file_path, const char *vertex_shader_path, const char *fragment_shader_path, size_t n, struct VertexAttribute attributes[]);
void Model_Free(struct Model model);

#endif // __MODELLOADER_H__