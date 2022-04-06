#include "UBO.h"

struct UBO UBO_Create(GLuint index, GLintptr offset, GLsizeiptr size)
{
    struct UBO ubo;

    glGenBuffers(1, &ubo.ID);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.ID);

    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // bind a buffer object to an indexed buffer target
    // glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo.ID);
    // or
    // GLintptr offset; // offset into this uniform, is all then start from 0
    // GLsizeiptr size; // size of data in uniform : 2 * sizeof(glm::mat4)
    glBindBufferRange(GL_UNIFORM_BUFFER, index, ubo.ID, offset, size);

    return ubo;
}

// Binds a shader to a UBO block
void UBO_Link_Shader(struct Shader shader, GLuint uniformBlockBinding, const char *uniformBlockName)
{
    const GLuint uniformBlockIndex = glGetUniformBlockIndex(shader.shader_id, uniformBlockName);
    glUniformBlockBinding(shader.shader_id, uniformBlockIndex, uniformBlockBinding);
}

void UBO_Data(struct UBO ubo, GLintptr offset, GLsizeiptr size, const void *data)
{
    // offset - offset into the element in the uniform
    // size   - size of the element(s) to add
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.ID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

    UBO_Unbind(); // Unbind
}

void UBO_Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
