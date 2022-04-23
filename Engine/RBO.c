#include "RBO.h"

RBO_t RBO_Create(const GLsizei width, const GLsizei height)
{
    // Create Render Buffer Object
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR Framebuffer is not complete!\n");

    return (RBO_t){.id = RBO};
}

RBO_t RBO_Create_Multi(const GLsizei samples, const GLsizei width, const GLsizei height)
{
    // Create Render Buffer Object
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "ERROR Framebuffer is not complete!\n");

    return (RBO_t){.id = RBO};
}