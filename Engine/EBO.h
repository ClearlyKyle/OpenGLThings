#ifndef __EBO_H__
#define __EBO_H__

#include "glad/glad.h"

struct EBO
{
    GLuint ID;
};

struct EBO EBO_Create();
void EBO_Destroy(struct EBO ebo);
void EBO_Bind(struct EBO ebo);
void EBO_Buffer(struct EBO ebo, GLsizeiptr size, void *data);

#endif // __EBO_H__