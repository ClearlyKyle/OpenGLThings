#include "Gamma.h"

static struct Gamma
{
    Camera_t    cam;
    struct Mesh model;
    FBO_t       msaa_fbo;
    FBO_t       post_processing_fbo;
} gam;

void Gamma_Init()
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

    // Generates shaders
    struct Shader shader_default = Shader_Create("../../Examples/19 - Gamma/default.vs",
                                                 "../../Examples/19 - Gamma/default.fs",
                                                 4,
                                                 (struct VertexAttribute[]){
                                                     {.index = 0, .name = "aPos"},
                                                     {.index = 1, .name = "aNormal"},
                                                     {.index = 2, .name = "aTex"},
                                                     {.index = 3, .name = "aColor"}});

    struct Shader shader_framebuffer = Shader_Create("../../Examples/19 - Gamma/framebuffer.vs",
                                                     "../../Examples/19 - Gamma/framebuffer.fs",
                                                     1,
                                                     (struct VertexAttribute[]){
                                                         {.index = 0, .name = "inPos"},
                                                         {.index = 1, .name = "inTexCoords"}});

    // Take care of all the light related things
    vec4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    vec3 lightPos   = {0.5f, 0.5f, 0.5f};

    // Controls the gamma function
    const float gamma = 2.2f;

    Shader_Bind(shader_default);
    Shader_Uniform_Vec4(shader_default, "lightColor", lightColor);
    Shader_Uniform_Vec3(shader_default, "lightPos", lightPos);

    Shader_Bind(shader_framebuffer);
    glUniform1i(glGetUniformLocation(shader_framebuffer.shader_id, "screenTexture"), 0);
    Shader_Uniform_Float(shader_framebuffer, "gamma", gamma);

    glEnable(GL_DEPTH_TEST); // Enables the Depth Buffer
    glEnable(GL_CULL_FACE);  // Enables Cull Facing
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    // Creates camera object
    struct Camera cam = Camera_Create(window.width, window.heigh, (vec3){0.0f, 0.0f, 2.0f}, 45.0f, 0.1f, 1000.0f);
    glm_vec3_copy((vec3){-11.172512f, 21.876404f, 33.018307f}, cam.position);
    glm_vec3_copy((vec3){0.302723f, -0.265558f, -0.915335f}, cam.orientation);
    cam.pitch = -15.400095f;
    cam.yaw   = -71.699715f;
    gam.cam   = cam;

    struct VAO rectVAO = VAO_Create();
    struct VBO rectVBO = VBO_Create(GL_ARRAY_BUFFER);
    VBO_Buffer(rectVBO, sizeof(rectangle_vertices), rectangle_vertices);
    VAO_Attr(rectVAO, rectVBO, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(0));
    VAO_Attr(rectVAO, rectVBO, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid *)(2 * sizeof(GLfloat)));

    FBO_t msaa_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D_MULTISAMPLE, window.width, window.heigh, 4);
    FBO_Add_RBO(&msaa_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    msaa_fbo.VAO = rectVAO.ID;
    gam.msaa_fbo = msaa_fbo;

    FBO_t post_processing_fbo = FBO_Create(shader_framebuffer, GL_TEXTURE_2D, window.width, window.heigh, 0);
    FBO_Add_RBO(&post_processing_fbo, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
    post_processing_fbo.VAO = rectVAO.ID;
    gam.post_processing_fbo = post_processing_fbo;

    // Load in models
    struct Mesh crow = Mesh_Load(shader_default, "../../Examples/res/models/crow/scene.gltf");
    gam.model        = crow;
}

void Gamma_Update()
{
    // Framebuffer_Draw_Init(gam.post_processing_fbo);
    Framebuffer_Draw_Init(gam.msaa_fbo);

    Camera_Inputs(&gam.cam);

    // TODO : Move this into the model Draw call
    Shader_Uniform_Vec3(gam.model.shader, "camPos", gam.cam.position);
    Camera_View_Projection_To_Shader(gam.cam, gam.model.shader, "camMatrix");

    Mesh_Draw(gam.model);

    Framebuffer_Update(gam.msaa_fbo, gam.post_processing_fbo);

    // Bind the default framebuffer
    Framebuffer_Draw(gam.post_processing_fbo);
}

void Gamma_OnExit()
{
    Camera_Print_Values(gam.cam);

    Mesh_Free(gam.model);
    Framebuffer_Destroy(&gam.msaa_fbo);
    Framebuffer_Destroy(&gam.post_processing_fbo);
}
