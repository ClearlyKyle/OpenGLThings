#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "util.h"

struct Texture
{
    // Texture slot and ID are not the same!
    GLuint ID;
    GLuint slot;
    GLenum type;
};

typedef struct Texture Texture_t;

Texture_t Texture_Create(const char *path, GLenum texture_type, GLuint slot, GLenum format, GLenum pixel_type);

void Texture_Bind(const struct Texture t);
void Texture_Unbind();

void Texture_Delete(const struct Texture t);

#endif // __TEXTURES_H__