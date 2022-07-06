#include "GBuffer.h"

GBuffer_t GBuffer_Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    GBuffer_t buf = {0};

    // Create the FBO
    glGenFramebuffers(1, &buf.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buf.fbo);

    // Create the gbuffer textures
    glGenTextures(GBUFFER_NUM_TEXTURES, buf.textures);

    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++)
    {
        glBindTexture(GL_TEXTURE_2D, buf.textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, buf.textures[i], 0);
    }

    // Depth
    glGenTextures(1, &buf.depthTexture);
    glBindTexture(GL_TEXTURE_2D, buf.depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, WindowWidth, WindowHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, buf.depthTexture, 0);

    // final
    glGenTextures(1, &buf.finalTexture);
    glBindTexture(GL_TEXTURE_2D, buf.finalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, buf.finalTexture, 0);

    // GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    // glDrawBuffers(3, DrawBuffers);

    const GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "GBuffer_Init error, status: 0x%x\n", Status);
        assert(false);
        // exit here?
    }

    // Restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return buf;
}

void GBuffer_StartFrame(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buf->fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer_BindForGeomPass(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buf->fbo);

    const GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0,
                                  GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2};

    glDrawBuffers(3, DrawBuffers);
}

void GBuffer_BindForWriting(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buf->fbo);
}

void GBuffer_BindForReading(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, buf->textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}

void GBuffer_SetReadBuffer(const enum GBUFFER_TEXTURE_TYPE TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

void GBuffer_BindForStencilPass()
{
    // Must disable the draw buffers
    glDrawBuffer(GL_NONE);
}

void GBuffer_BindForLightPass(const GBuffer_t *const buf)
{
    glDrawBuffer(GL_COLOR_ATTACHMENT4);

    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, buf->textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}

void GBuffer_BindForFinalPass(const GBuffer_t *const buf)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, buf->fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
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