#include "VAO.h"

// Create and Bind VAO
struct VAO VAO_Create()
{
    struct VAO vao;
    glGenVertexArrays(1, &vao.ID);
    VAO_Bind(vao);
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

void VAO_Unbind()
{
    glBindVertexArray(0);
}

// size     - components per generic vertex attribute
// type     - data type of each component in the array
// stride   - byte offset between consecutive generic vertex attribute
// offset   - offset of the first component of the first generic vertex attribute
void VAO_Attr(struct VAO vao, struct VBO vbo, GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *offset)
{
    VAO_Bind(vao);
    VBO_Bind(vbo);

    // NEWER OpenGL Method
    // const GLuint attrib_location = glGetAttribLocation(shader.id, name);
    // glVertexAttribPointer(attrib_location, size, type, GL_FALSE, stride, offset);
    // glEnableVertexAttribArray(attrib_location);

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, offset);

    // VAO_Unbind();
    // VBO_Unbind();
}