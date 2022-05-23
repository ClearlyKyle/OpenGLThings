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

static void _Check_Framebuffer_Status()
{
    // check FBO status
    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        fprintf(stderr, "[ERROR] Framebuffer incomplete: Attachment is NOT complete\n");
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        fprintf(stderr, "[ERROR] Framebuffer incomplete: No image is attached to FBO\n");
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        fprintf(stderr, "[ERROR] Framebuffer incomplete: Draw buffer\n");
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        fprintf(stderr, "[ERROR] Framebuffer incomplete: Read buffer\n");
        break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        fprintf(stderr, "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation\n");
        break;

    default:
        fprintf(stderr, "[ERROR] Framebuffer incomplete: Unknown error\n");
        break;
    }
}

// target  : GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_2D_MULTISAMPLE ...
// samples : used for MSAA with GL_TEXTURE_2D_MULTISAMPLE
FBO_t FBO_Create(Shader_t shader, GLenum target, const GLsizei width, const GLsizei height, GLsizei samples)
{
    // Create Frame Buffer Object
    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    CHECK_GL_ERRORS;

    // Create Framebuffer Texture
    GLuint framebuffer_texture;
    glGenTextures(1, &framebuffer_texture);
    glBindTexture(target, framebuffer_texture);
    CHECK_GL_ERRORS;

    if (samples > 0 && target == GL_TEXTURE_2D_MULTISAMPLE)
    {
        GLint maximum_number_samples = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &maximum_number_samples);
        if (samples > maximum_number_samples)
            samples = maximum_number_samples;
        else if (samples % 2 != 0)
            samples--;

        // GL_TEXTURE_2D_MULTISAMPLE for AA FB
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebuffer_texture, 0);
    }
    else
    {
        glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, framebuffer_texture, 0);
    }

    // Error checking framebuffer
    _Check_Framebuffer_Status();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FBO_t return_fbo   = {0};
    return_fbo.shader  = shader;
    return_fbo.FBO     = FBO;
    return_fbo.FBO_tex = framebuffer_texture;

    return_fbo.msaa_samples = samples;
    return_fbo.width        = width;
    return_fbo.height       = height;

    return return_fbo;
}

// internalformat :     color-renderable (GL_RGB, GL_RGBA, etc.), https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glRenderbufferStorage.xhtml
//                      depth-renderable (GL_DEPTH_COMPONENT),
//                      or stencil-renderable formats (GL_STENCIL_INDEX)
// attachment     :     GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT...
void FBO_Add_RBO(FBO_t *fbo, GLenum internalformat, GLenum attachment)
{
    FBO_Bind(*fbo);

    // Check width and height
    GLint max_size;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_size);
    check_that((fbo->width < max_size && fbo->height < max_size), "GL_MAX_RENDERBUFFER_SIZE error, Max Size = %d\n", max_size);

    // Create Render Buffer Object
    GLuint RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    if (fbo->msaa_samples > 0)
    {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, fbo->msaa_samples, internalformat, fbo->width, fbo->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, RBO);
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, internalformat, fbo->width, fbo->height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, RBO);
    }

    fbo->RBO = RBO;
}

// explicitly blit color/depth buffer from multi-sample fbo to single-sample fbo
// this call is necessary for the single-sample color/depth buffer to be updated
void Framebuffer_Update(const FBO_t msaa_fbo, const FBO_t post_processing_fbo)
{
    if (msaa_fbo.msaa_samples > 0)
    {
        // Make it so the multisampling FBO is read while the post-processing FBO is drawn
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_fbo.FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, post_processing_fbo.FBO);

        // Conclude the multisampling and copy it to the post-processing FBO
        const GLint width  = msaa_fbo.width;
        const GLint height = msaa_fbo.height;

        glBlitFramebuffer(0, 0, width, height,
                          0, 0, width, height,
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        // NOTE: blit separately depth buffer because different scale filter
        // glBlitFramebuffer(0, 0, width, height,
        //                  0, 0, width, height,
        //                  GL_DEPTH_BUFFER_BIT,
        //                  GL_NEAREST);
    }

    // also, generate mipmaps for color buffer (texture)
    // glBindTexture(GL_TEXTURE_2D, msaa_fbo.FBO_tex);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

void FBO_Bind(const FBO_t fbo)
{
    // GL_FRAMEBUFFER binding target simply sets both the read and the write to the same FBO.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.FBO);
}

void FBO_UnBind(const FBO_t fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer_Draw_Init(struct FBO fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Framebuffer_Draw(struct FBO fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the default framebuffer

    // Draw the framebuffer rectangle
    Shader_Bind(fbo.shader);

    glBindVertexArray(fbo.VAO);
    glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo.FBO_tex);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);
}

void Framebuffer_Destroy(struct FBO *fbo)
{
    if (fbo->RBO)
    {
        glDeleteRenderbuffers(1, &fbo->RBO);
        fbo->RBO = 0;
    }
    if (fbo->FBO)
    {
        glDeleteFramebuffers(1, &fbo->FBO);
        fbo->FBO = 0;
    }
    if (fbo->FBO_tex)
    {
        glDeleteTextures(1, &fbo->FBO_tex);
        fbo->FBO_tex = 0;
    }
    if (fbo->VAO)
    {
        glDeleteVertexArrays(1, &fbo->VAO);
        fbo->VAO = 0;
    }
}

static struct Debug_FBO
{
    GLuint VAO_id; // Used for debug quad
    GLuint shader_id;
} debug_fbo;

void Debug_FBO_Init()
{
    const GLfloat quad_verticies[] = {
        // positions        // texture Coords
        0.5f, 0.9f, 0.0f, 1.0f, //
        0.9f, 0.9f, 1.0f, 1.0f, //
        0.5f, 0.5f, 0.0f, 0.0f, //
        0.9f, 0.5f, 1.0f, 0.0f, //
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verticies), &quad_verticies, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

    debug_fbo.VAO_id = VAO;

    // Create Shader
    const Shader_t shader = Shader_Create("../../Engine/shaders/fbo_debug.vs",
                                          "../../Engine/shaders/fbo_debug.fs",
                                          2,
                                          (struct VertexAttribute[]){
                                              {.index = 0, .name = "position"},
                                              {.index = 1, .name = "texCoords"}});

    Shader_Bind(shader);
    Shader_Uniform_Int(shader, "fboAttachment", 0);

    debug_fbo.shader_id = shader.shader_id;
}

void Debug_FBO_Draw(const GLuint fbo_tex)
{
    if (!(debug_fbo.shader_id || debug_fbo.VAO_id))
    {
        puts("You forgot to call Debug_FBO_Init()!");
        return;
    }
    glUseProgram(debug_fbo.shader_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_tex);

    glBindVertexArray(debug_fbo.VAO_id);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // glBindVertexArray(0);
}