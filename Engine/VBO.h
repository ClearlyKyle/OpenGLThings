#ifndef __VBO_H__
#define __VBO_H__

#include "util.h"

struct VBO
{
    GLuint ID;
    GLint type;
};

struct VBO VBO_Create(GLint type);
void VBO_Destroy(struct VBO vbo);
void VBO_Bind(struct VBO vbo);
void VBO_Unbind();
void VBO_Buffer(struct VBO vbo, GLsizeiptr size, const GLvoid *data);

#endif // __VBO_H__