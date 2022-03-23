#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "glad/glad.h"

struct Shader
{
    GLuint shader_id;
    GLuint vs_handle;
    GLuint fs_handle;
};

void validate_shader(GLuint shader, const char *file);
bool validate_program(GLuint program);

#endif // __SHADERS_H__