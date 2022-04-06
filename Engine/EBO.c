#include "EBO.h"

struct EBO EBO_Create()
{
    struct EBO ebo = {0};

    glGenBuffers(1, &ebo.ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.ID);

    return ebo;
}

void EBO_Destroy(struct EBO ebo)
{
    glDeleteBuffers(1, &ebo.ID);
}

void EBO_Bind(struct EBO ebo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.ID);
}

void EBO_Unbind()
{
    // make sure you don't unbind the element array buffer before unbinding your VAO
    // otherwise it doesn't have an EBO configured
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO_Buffer(struct EBO vbo, GLsizeiptr size, const GLvoid *data)
{
    EBO_Bind(vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    // EBO_Unbind();
}
