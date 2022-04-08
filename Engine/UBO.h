#ifndef __UBO_H__
#define __UBO_H__

#include "util.h"

#include "Shaders.h"

struct UBO
{
    GLuint ID;
};

struct UBO UBO_Create(GLuint index, GLintptr offset, GLsizeiptr size);

void UBO_Bind_Buffer_To_Index(GLuint ubo_ID, GLuint index, GLintptr offset, GLsizeiptr size);
void UBO_Link_Shader(struct Shader shader, GLuint uniformBlockBinding, const char *uniformBlockName);
void UBO_Data(struct UBO ubo, GLintptr offset, GLsizeiptr size, const void *data);

void UBO_Unbind();

#endif // __UBO_H__