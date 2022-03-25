#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "util.h"

struct Texture
{
    GLuint ID;
    GLenum type;
};

struct Texture Texture_Create(const char *path, GLenum texture_type, GLenum slot, GLenum format, GLenum pixel_type);

void Texture_Uniform(const struct Shader shader, const char *uniform, GLuint unit);

void Texture_Bind(const struct Texture t);
void Texture_Unbind();

void Texture_Delete(const struct Texture t);

#endif // __TEXTURES_H__