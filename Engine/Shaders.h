#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "glad/glad.h"

// VertexAttribute - are the (locations = 0) in the vs, for example:
// layout (location = 0) in vec3 position;    "index = 0, name = position"
// layout (location = 1) in vec2 uv;          "index = 1, name = uv"
struct VertexAttribute
{
    GLuint index;
    const GLchar *name;
};

struct Shader
{
    GLuint shader_id;
    GLuint vs_handle;
    GLuint fs_handle;
};

// Shader Functions
struct Shader Shader_Create(char *vertex_shader_path, char *fragment_shader_path, size_t n, struct VertexAttribute attributes[]);
void Shader_Destroy(const struct Shader shader);
void Shader_Bind(const struct Shader shader);

// Uniforms
void Shader_Uniform_Float(struct Shader shader, const char *name, float f);

#endif // __SHADERS_H__