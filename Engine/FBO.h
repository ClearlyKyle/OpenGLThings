#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Shaders.h"
#include "RBO.h"

struct FBO
{
    GLuint FBO;
    GLuint FBO_tex;
    GLuint VAO;

    GLsizei width;
    GLsizei height;

    Shader_t shader;
};

typedef struct FBO FBO_t;

FBO_t FBO_Texture2D(Shader_t shader, GLenum target, const GLsizei width, const GLsizei height);
// FBO_t  Framebuffer_2D(struct Shader shader, GLenum target, GLsizeiptr size, const GLfloat *verts, const GLsizei width, const GLsizei height);
// FBO_t  Framebuffer_Multisample(struct Shader shader, GLenum target, GLsizeiptr size, GLfloat *verts, GLsizei width, GLsizei height, GLsizei samples);

void FBO_Add_RBO(FBO_t fbo);
void Framebuffer_Bind(struct FBO fbo);
void Framebuffer_Draw(struct FBO fbo);

void Framebuffer_Destroy(struct FBO fbo);

#endif // __FRAMEBUFFER_H__