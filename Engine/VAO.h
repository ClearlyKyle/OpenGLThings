#ifndef __VAO_H__
#define __VAO_H__

#include "util.h"

#include "VBO.h"

struct VAO
{
    GLuint ID;
};

struct VAO VAO_Create();
void VAO_Destroy(struct VAO vao);
void VAO_Bind(struct VAO vao);
void VAO_Unbind();
void VAO_Attr(struct VAO vao, struct VBO vbo, GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *offset);
#endif // __VAO_H__