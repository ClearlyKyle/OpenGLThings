#include "VBO.h"

struct VBO VBO_Create(GLint type)
{
    struct VBO vbo = {0};
    vbo.type = type;

    // 1 - number of buffer object names to be generated
    // an array in which the generated buffer object names are stored
    glGenBuffers(1, &vbo.ID);

    return vbo;
}

void VBO_Destroy(struct VBO vbo)
{
    glDeleteBuffers(1, &vbo.ID);
}

void VBO_Bind(struct VBO vbo)
{
    glBindBuffer(vbo.type, vbo.ID);
}

void VBO_Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO_Buffer(struct VBO vbo, GLsizeiptr size, const GLvoid *data)
{
    VBO_Bind(vbo);
    glBufferData(vbo.type, size, data, GL_STATIC_DRAW);
    //VBO_Unbind();
}
