#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Shaders.h"

struct FBO
{
    GLuint FBO;
    GLuint FBO_tex;

    GLuint VAO;
    GLuint RBO;

    GLsizei width;
    GLsizei height;
    GLsizei msaa_samples;

    Shader_t shader;

    GLenum type;
};

typedef struct FBO FBO_t;

FBO_t FBO_Create(Shader_t shader, GLenum target, GLenum internalformat, const GLsizei width, const GLsizei height, GLsizei samples);
void  FBO_Add_RBO(FBO_t *fbo, GLenum internalformat, GLenum attachment);

void Framebuffer_Update(const FBO_t msaa_fbo, const FBO_t post_processing_fbo);

void FBO_Bind(const FBO_t fbo);
void FBO_UnBind(const FBO_t fbo);

void Framebuffer_Draw_Init(struct FBO fbo);
void Framebuffer_Draw(struct FBO fbo);

void Framebuffer_Destroy(struct FBO *fbo);

void Debug_FBO_Init();
void Debug_FBO_Draw(const GLuint fbo_tex);

#endif // __FRAMEBUFFER_H__