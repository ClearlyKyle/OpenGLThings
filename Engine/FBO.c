#include "FBO.h"

static void _Standard_Rect()
{
    const GLfloat rectangle_vertices[] =
        {
            //  Coords   // texCoords
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f};
}

FBO_t FBO_Texture2D(Shader_t shader, GLenum target, const GLsizei width, const GLsizei height)
{
    // Create Frame Buffer Object
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create Framebuffer Texture
    GLuint framebuffer_texture;
    glGenTextures(1, &framebuffer_texture);
    glBindTexture(target, framebuffer_texture);

    glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, framebuffer_texture, 0);

    // Error checking framebuffer
    check_that(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, "Framebuffer status is not good :(\n");
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     fprintf(stderr, "ERROR Framebuffer is not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FBO_t return_fbo;
    return_fbo.shader  = shader;
    return_fbo.FBO     = FBO;
    return_fbo.FBO_tex = framebuffer_texture;

    return_fbo.width  = width;
    return_fbo.height = height;

    return return_fbo;
}

FBO_t FBO_2DMultisample(Shader_t shader, GLenum target, const GLsizei samples, const GLsizei width, const GLsizei height)
{
    // > We have to attach at least one buffer (color, depth or stencil buffer).
    // > There should be at least one colour attachment.
    // > All attachments should be complete as well (reserved memory).
    // > Each buffer should have the same number of samples.

    // Create Frame Buffer Object
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create Framebuffer Texture
    GLuint framebuffer_texture;
    glGenTextures(1, &framebuffer_texture);
    glBindTexture(target, framebuffer_texture);

    // GL_TEXTURE_2D_MULTISAMPLE for AA FB
    glTexImage2DMultisample(target, samples, GL_RGB, width, height, GL_TRUE);

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebuffer_texture, 0);

    // Create Render Buffer Object
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR Framebuffer is not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FBO_t return_fbo;
    return_fbo.shader  = shader;
    return_fbo.FBO     = FBO;
    return_fbo.FBO_tex = framebuffer_texture;

    return_fbo.width  = width;
    return_fbo.height = height;

    return return_fbo;
}

void FBO_Add_RBO(FBO_t fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.FBO);

    // Create Render Buffer Object
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fbo.width, fbo.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR Framebuffer is not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer_Bind(struct FBO fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.FBO);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); // Enable depth testing since it's disabled when drawing the framebuffer rectangle
}

void Framebuffer_Draw(struct FBO fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the default framebuffer

    // Draw the framebuffer rectangle
    Shader_Bind(fbo.shader);
    glBindVertexArray(fbo.VAO);
    glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
    glBindTexture(GL_TEXTURE_2D, fbo.FBO_tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Framebuffer_Destroy(struct FBO fbo)
{
    glDeleteFramebuffers(1, &fbo.FBO);
}