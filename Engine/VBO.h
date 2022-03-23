#ifndef __VBO_H__
#define __VBO_H__

#include <stdbool.h>

#include "glad/glad.h"

struct VBO
{
    GLuint ID;
    GLint type;
};

struct VBO VBO_Create(GLint type, bool dynamic);
void VBO_Destroy(struct VBO vbo);
void VBO_Bind(struct VBO vbo);
void VBO_Buffer(struct VBO vbo, GLsizeiptr size, void *data);

#endif // __VBO_H__