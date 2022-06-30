#include "GBuffer.h"

GBuffer_t GBuffer_Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    GBuffer_t buf = {0};

    // Create the FBO
    glGenFramebuffers(1, &buf.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buf.fbo);

    // Create the gbuffer textures
    glGenTextures(GBUFFER_NUM_TEXTURES, buf.textures);
    glGenTextures(1, &buf.depthTexture);

    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++)
    {
        glBindTexture(GL_TEXTURE_2D, buf.textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, buf.textures[i], 0);
    }

    // Depth
    glBindTexture(GL_TEXTURE_2D, buf.depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buf.depthTexture, 0);

    GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "GBuffer_Init error, status: 0x%x\n", Status);
        // exit here?
    }

    // Restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return buf;
}

void GBuffer_BindForWriting(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buf->fbo);
}

void GBuffer_BindForReading(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, buf->fbo);
}

void GBuffer_SetReadBuffer(const enum GBUFFER_TEXTURE_TYPE TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

void GBuffer_Destroy(GBuffer_t *buf)
{
    if (buf->fbo != 0)
    {
        glDeleteFramebuffers(1, &buf->fbo);
    }

    if (buf->textures[0] != 0)
    {
        glDeleteTextures(GBUFFER_NUM_TEXTURES, buf->textures);
    }

    if (buf->depthTexture != 0)
    {
        glDeleteTextures(1, &buf->depthTexture);
    }
}