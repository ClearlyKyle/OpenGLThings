#include "VAO.h"

struct VAO VAO_Create()
{
    struct VAO vao;
    glGenVertexArrays(1, &vao.ID);
    return vao;
}

void VAO_Destroy(struct VAO vao)
{
    glDeleteVertexArrays(1, &vao.ID);
}

void VAO_Bind(struct VAO vao)
{
    glBindVertexArray(vao.ID);
}

static void VAO_Unbind()
{
    glBindVertexArray(0);
}

void VAO_Attr(struct VAO vao, struct VBO vbo, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset)
{
    VAO_Bind(vao);
    VBO_Bind(vbo);

    // glVertexAttribIPointer(index, size, type, stride, (void *)offset);
    // glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)offset);

    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(index);
    VAO_Unbind();
}