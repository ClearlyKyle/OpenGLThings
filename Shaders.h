#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "glad/glad.h"

struct Shader
{
    GLuint shader_id;
    GLuint vs_handle;
    GLuint fs_handle;
};

struct Shader Shader_Create(char *vertex_shader_path, char *fragment_shader_path, size_t n, struct VertexAttr attributes[]);
void Shader_Destroy(const struct Shader shader);
void Shader_Bind(const struct Shader shader);

#endif // __SHADERS_H__