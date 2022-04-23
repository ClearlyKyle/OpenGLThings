#ifndef __RBO_H__
#define __RBO_H__

#include "util.h"

struct RBO
{
    GLuint id;
};

typedef struct RBO RBO_t;

RBO_t RBO_Create(const GLsizei width, const GLsizei height);
RBO_t RBO_Create_Multi(const GLsizei samples, const GLsizei width, const GLsizei height);

#endif // __RBO_H__