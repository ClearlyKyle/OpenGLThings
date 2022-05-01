#include "shadowmap.h"

struct Shadowmap Shadowmap_Create(struct Shader shader)
{
    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH  = 1024;
    const unsigned int SHADOW_HEIGHT = 1024;

    GLuint depth_map_FBO;
    glGenFramebuffers(1, &depth_map_FBO);

    // create depth texture
    GLuint depth_map_tex;
    glGenTextures(1, &depth_map_tex);
    glBindTexture(GL_TEXTURE_2D, depth_map_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_tex, 0);

    // Disable writes to the color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    const GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("FB error, status: 0x%x\n", Status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return (struct Shadowmap){
        .FBO_Id = depth_map_FBO,
        .tex_Id = depth_map_tex,
        .height = SHADOW_HEIGHT,
        .width  = SHADOW_WIDTH,
        .shader = shader};
}