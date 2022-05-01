#ifndef __SHADOWMAP_H__
#define __SHADOWMAP_H__

#include "util.h"

#include "Shaders.h"

struct Shadowmap
{
    GLuint        FBO_Id;
    GLuint        tex_Id;
    GLuint        width;
    GLuint        height;
    struct Shader shader;
};

typedef struct Shadowmap Shadowmap_t;

Shadowmap_t Shadowmap_Create(struct Shader shader);

#endif // __SHADOWMAP_H__