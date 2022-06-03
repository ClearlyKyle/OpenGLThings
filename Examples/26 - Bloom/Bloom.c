#include "Bloom.h"

static struct Bloom
{
    Camera_t cam;

    VAO_t     plane_VAO;
    Texture_t plane_tex_DIFFUSE;
    Texture_t plane_tex_NORMAL;
    Texture_t plane_tex_DISP;
    Shader_t  plane_SHADER;

    Shader_t shader_program;
    Shader_t shader_framebuffer;
    Shader_t shader_blur;

    VAO_t rect_VAO;

    GLuint postProcessingFBO;
    GLuint postProcessingTexture;
    GLuint bloomTexture;

    FBO_t ping_pong_FBO[2];
} b;

void Bloom_Init()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE); // Enables Multisampling

    glEnable(GL_CULL_FACE); // Enables Cull Facing
    glCullFace(GL_FRONT);   // Keeps front faces
    glFrontFace(GL_CCW);    // Uses counter clock-wise standard

    const struct Shader shader_program = Shader_Create2("../../Examples/26 - Bloom/default.vs",
                                                        "../../Examples/26 - Bloom/default.fs",
                                                        "../../Examples/26 - Bloom/default.gs",
                                                        2,
                                                        (struct VertexAttribute[]){
                                                            {.index = 0, .name = "aPos"},
                                                            {.index = 1, .name = "aNormal"},
                                                            {.index = 2, .name = "aColor"},
                                                            {.index = 3, .name = "aTex"},
                                                        });

    const struct Shader shader_framebuffer = Shader_Create("../../Examples/26 - Bloom/framebuffer.vs",
                                                           "../../Examples/26 - Bloom/framebuffer.fs",
                                                           2,
                                                           (struct VertexAttribute[]){
                                                               {.index = 0, .name = "aPos"},
                                                               {.index = 1, .name = "inTexCoords"},
                                                           });

    const struct Shader shader_blur = Shader_Create("../../Examples/26 - Bloom/framebuffer.vs",
                                                    "../../Examples/26 - Bloom/blur.fs",
                                                    2,
                                                    (struct VertexAttribute[]){
                                                        {.index = 0, .name = "aPos"},
                                                        {.index = 1, .name = "inTexCoords"},
                                                    });

    const Camera_t cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    b.cam              = cam;

    b.shader_program     = shader_program;
    b.shader_framebuffer = shader_framebuffer;
    b.shader_blur        = shader_blur;

    vec4        light_colour   = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3        light_position = {0.5f, 0.5f, 0.5f};
    const float gamma          = 2.2f;

    Shader_Bind(shader_program);
    Shader_Uniform_Vec4(shader_program, "lightColor", light_colour);
    Shader_Uniform_Vec3(shader_program, "lightPos", light_position);

    Shader_Bind(shader_framebuffer);
    Shader_Uniform_Int(shader_framebuffer, "screenTexture", 0);
    Shader_Uniform_Int(shader_framebuffer, "bloomTexture", 1);
    Shader_Uniform_Float(shader_framebuffer, "gamma", gamma);

    Shader_Bind(shader_blur);
    Shader_Uniform_Int(shader_blur, "screenTexture", 0);

    { // RECT for FBO

        const GLfloat rectangle_vertices[] =
            {
                //  Coords   // texCoords
                1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 1.0f,

                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 1.0f};

        const VAO_t rect_VAO = VAO_Create();
        const VBO_t rect_VBO = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(rect_VBO, sizeof(rectangle_vertices), (const GLvoid *)rectangle_vertices);
        VAO_Attr(rect_VAO, rect_VBO, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(0));
        VAO_Attr(rect_VAO, rect_VBO, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

        b.rect_VAO = rect_VAO;
    } // RECT for FBO

    { // CREATE FBO for BLOOM and POST-PROCESSING

        GLuint postProcessingFBO;
        glGenFramebuffers(1, &postProcessingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

        // Create Framebuffer Texture
        GLuint postProcessingTexture;
        glGenTextures(1, &postProcessingTexture);
        glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window.width, window.heigh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

        // Create Second Framebuffer Texture
        GLuint bloomTexture;
        glGenTextures(1, &bloomTexture);
        glBindTexture(GL_TEXTURE_2D, bloomTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window.width, window.heigh, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);
        // Node here the GL_COLOR_ATTACHMENT1 and not GL_COLOR_ATTACHMENT0

        // Tell OpenGL we need to draw to both attachments
        const GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);

        // Error checking framebuffer
        const GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
            printf("Post-Processing Framebuffer error: %d\n", fbo_status);

        b.postProcessingFBO     = postProcessingFBO;
        b.postProcessingTexture = postProcessingTexture;
        b.bloomTexture          = bloomTexture;

    } // CREATE FBO for BLOOM and POST-PROCESSING

    // Create Ping Pong Framebuffers for repetitive blurring
    b.ping_pong_FBO[0] = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, GL_RGBA16F, window.width, window.heigh, 0);
    b.ping_pong_FBO[1] = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, GL_RGBA16F, window.width, window.heigh, 0);

    { // SETUP PLANE
        const GLfloat plane_vertices[] =
            {
                //     COORDINATES  / NORMALS /     COLORS      / TexCoord  //
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};

        const GLuint plane_indicies[] =
            {
                0, 1, 2, // Upper triangle
                0, 2, 3  // Lower triangle
            };

        const struct VAO VAO = VAO_Create();
        const struct VBO VBO = VBO_Create(GL_ARRAY_BUFFER);
        VBO_Buffer(VBO, sizeof(plane_vertices), (const GLvoid *)plane_vertices);

        const struct EBO ebo = EBO_Create();
        EBO_Buffer(ebo, sizeof(plane_indicies), (const GLvoid *)plane_indicies);

        VAO_Attr(VAO, VBO, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)0);
        VAO_Attr(VAO, VBO, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(3 * sizeof(GLfloat)));
        VAO_Attr(VAO, VBO, 2, 3, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(6 * sizeof(GLfloat)));
        VAO_Attr(VAO, VBO, 3, 2, GL_FLOAT, 11 * sizeof(GLfloat), (const GLvoid *)(9 * sizeof(GLfloat)));

        const char    *diffuse_file_path      = "../../Examples/res/textures/bloom/diffuse.png";
        const char    *normal_file_path       = "../../Examples/res/textures/bloom/normal.png";
        const char    *displacement_file_path = "../../Examples/res/textures/bloom/displacement.png";
        struct Texture diff_tex               = Texture_Create(diffuse_file_path, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
        struct Texture nrm_tex                = Texture_Create(normal_file_path, GL_TEXTURE_2D, 1, GL_RGB, GL_UNSIGNED_BYTE);
        struct Texture disp_tex               = Texture_Create(displacement_file_path, GL_TEXTURE_2D, 2, GL_RED, GL_UNSIGNED_BYTE);

        vec3   translation = {0.0f, 0.0f, 0.0f};
        versor rotation    = {0.0f, 0.0f, 0.0f, 0.0f};
        vec3   scale       = {1.0f, 1.0f, 1.0f};

        mat4 object_translation = {0};
        mat4 object_rotaion     = {0};
        mat4 object_scale       = {0};
        mat4 object_matrix      = GLM_MAT4_IDENTITY_INIT;

        glm_translate_make(object_translation, translation);
        glm_quat_mat4(rotation, object_rotaion);
        glm_scale_make(object_scale, scale);

        Shader_Bind(shader_program);
        Shader_Uniform_Vec4(shader_program, "lightColor", light_colour);
        Shader_Uniform_Vec3(shader_program, "lightPos", light_position);
        Shader_Uniform_Mat4(shader_program, "translation", object_translation);
        Shader_Uniform_Mat4(shader_program, "rotation", object_rotaion);
        Shader_Uniform_Mat4(shader_program, "scale", object_scale);
        Shader_Uniform_Mat4(shader_program, "model", object_matrix);
        Shader_Uniform_Int(shader_program, "diffuse0", 0);
        Shader_Uniform_Int(shader_program, "normal0", 1);
        Shader_Uniform_Int(shader_program, "displacement0", 2);

        b.plane_VAO         = VAO;
        b.plane_tex_DIFFUSE = diff_tex;
        b.plane_tex_NORMAL  = nrm_tex;
        b.plane_tex_DISP    = disp_tex;
        b.plane_SHADER      = shader_program;
    } // SETUP PLANE
}

static bool first_iteration;

void Bloom_Update()
{
    glBindFramebuffer(GL_FRAMEBUFFER, b.postProcessingFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Camera_Inputs(&b.cam);

    { // DRAW PLANE
        Shader_Bind(b.plane_SHADER);
        Shader_Uniform_Vec3(b.plane_SHADER, "camPos", b.cam.position);
        Camera_View_Projection_To_Shader(b.cam, b.plane_SHADER, "camMatrix");

        Texture_Bind(b.plane_tex_DIFFUSE);
        Texture_Bind(b.plane_tex_NORMAL);
        Texture_Bind(b.plane_tex_DISP);

        VAO_Bind(b.plane_VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    } // DRAW PLANE

    // Bounce the image data around to blur multiple times
    bool horizontal = true;
    first_iteration = true;

    // Amount of time to bounce the blur
    const unsigned int amount = 2;

    Shader_Bind(b.shader_blur);
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, b.ping_pong_FBO[horizontal].FBO);
        Shader_Uniform_Int(b.shader_blur, "horizontal", horizontal);

        // In the first bounc we want to get the data from the bloomTexture
        if (first_iteration)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, b.bloomTexture);
            first_iteration = false;
        }
        // Move the data between the pingPong textures
        else
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, b.ping_pong_FBO[!horizontal].FBO_tex);
        }

        // Render the image
        VAO_Bind(b.rect_VAO);

        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Switch between vertical and horizontal blurring
        horizontal = !horizontal;
    }

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Draw the framebuffer rectangle
    Shader_Bind(b.shader_framebuffer);

    VAO_Bind(b.rect_VAO);

    glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, b.postProcessingTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, b.ping_pong_FBO[!horizontal].FBO_tex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Bloom_OnExit()
{
    Camera_Print_Values(b.cam);

    VAO_Destroy(b.plane_VAO);
    Texture_Delete(b.plane_tex_DIFFUSE);
    Texture_Delete(b.plane_tex_NORMAL);
    Texture_Delete(b.plane_tex_DISP);

    Shader_Destroy(&b.shader_program);
    Shader_Destroy(&b.shader_framebuffer);
    Shader_Destroy(&b.shader_blur);
    Shader_Destroy(&b.plane_SHADER);

    VAO_Destroy(b.rect_VAO);
}
